import scene_system as sc
import math

class NearestPoint(object):

    def __init__(self, sample, found, distance_squared=None):
        self.sample = sample
        self.found = found
        self.distance_squared = distance_squared

class PathPart(object):

    def At(self, sample):
        return sc.Pose()

    def FindNearest(self, location, return_distance_squared=False):
        return NearestPoint(
            sample=0,
            found=False,
            distance_squared=0 if return_distance_squared else None)

    def AsPath(self):
        return Path((self,))


class Line(PathPart):

    def __init__(self, p0, p1, tube_radius, o0 = sc.Quaternion.Identity(), o1 = None):
        self.start = p0
        delta = (p1 - p0)
        self.direction = delta.GetNormalized()
        self.length = delta.GetLength()
        self.radius_sq = pow(tube_radius, 2)
        self.start_orientation = o0
        if o1 is not None:
            self.delta_rotation = o0.Inverse() * o1
        else:
            self.delta_rotation = sc.Quaternion.Identity()

    def At(self, t):
        return sc.Pose(self.start + self.direction * (t * self.length), self.start_orientation * self.delta_rotation.ToPower(t))

    def FindNearest(self, location, return_distance_squared=False):
        b = location - self.start
        projected_length = b.Dot(self.direction)
        sample = projected_length / self.length

        if sample < 0:
            sample = 0
            dist_squared = b.GetLengthSquared()
        elif sample > 1:
            sample = 1
            dist_squared = (
                b -
                self.direction *
                self.length).GetLengthSquared()
        else:
            dist_squared = b.GetLengthSquared() - pow(projected_length, 2)

        return NearestPoint(
            sample, dist_squared <= self.radius_sq, dist_squared)


class Path(PathPart):

    def __init__(self, paths, circular=False, path_lengths=None):
        assert(len(paths) > 0)

        self.paths = paths
        self.circular = circular
        self.AssertConnected()

        if path_lengths is None:
            path_lengths = (1 / len(paths),) * len(self.paths)
        cutoff = 0
        self.cutoffs = [path_lengths[0], ] * len(self.paths)
        for i in range(1, len(self.paths)):
            self.cutoffs[i] = self.cutoffs[i - 1] + path_lengths[i]
        assert(math.isclose(self.cutoffs[-1], 1))

    def AssertConnected(self):
        for i in range(1, len(self.paths)):
            assert(self.paths[i - 1].At(1) == self.paths[i].At(0))
        if self.circular:
            assert(self.paths[0].At(0) == self.paths[-1].At(1))

    def CreateSamples(self, path_sample_rates=None, path_sample_rate=5):
        if path_sample_rates is None:
            path_sample_rates = (path_sample_rate,) * len(self.paths)
        else:
            assert(len(path_sample_rates) == len(self.paths))

        path_vertices = [sc.ArrayFloat3(self.paths[0].At(0).location)]
        for path, sample_rate in zip(self.paths, path_sample_rates):
            for sample_fraction in range(0, sample_rate):
                vertex_location = path.At((sample_fraction + 1) / sample_rate).location
                path_vertices.append(sc.ArrayFloat3(vertex_location))
        return path_vertices

    def FindNearest(self, location, return_distance_squared=False):
        nearest = NearestPoint(
            sample=float('inf'),
            found=False,
            distance_squared=float('inf'))
        previous_cutoff = 0
        for path, cutoff in zip(self.paths, self.cutoffs):
            sub_nearest = path.FindNearest(
                location, return_distance_squared=True)
            if sub_nearest.found and sub_nearest.distance_squared < nearest.distance_squared:
                nearest = sub_nearest
                nearest.sample = sub_nearest.sample * \
                    (cutoff - previous_cutoff) + previous_cutoff
            previous_cutoff = cutoff

        return nearest

    def At(self, sample):
        subpath_index = 0
        previous_cutoff = 0
        while self.cutoffs[subpath_index] < sample:
            previous_cutoff = self.cutoffs[subpath_index]
            subpath_index += 1
        next_cutoff = self.cutoffs[subpath_index]

        return self.paths[subpath_index].At(
            (sample - previous_cutoff) / (next_cutoff - previous_cutoff))


class CirclePath(Path):
    def __init__(self, radius, center, right_vec, up_vec, num_samples, tube_radius = 0.1, *args, **kwargs):
        right_vec = right_vec.GetNormalized() * radius
        up_vec = up_vec.GetNormalized() * radius
        normal = up_vec.Cross(right_vec).GetNormalized()
        theta_samples = tuple(
            math.pi * 2 * i / num_samples for i in range(
                num_samples + 1))
        lines = tuple(
            Line(
                p0 = center + math.cos(theta_samples[i]) * right_vec + math.sin(theta_samples[i]) * up_vec,
                p1 = center + math.cos(theta_samples[i+1]) * right_vec + math.sin(theta_samples[i+1]) * up_vec,
                o0 = sc.Quaternion.RotationAboutLocation(normal, theta_samples[i]),
                o1 = sc.Quaternion.RotationAboutLocation(normal, theta_samples[i+1]),
                tube_radius = tube_radius) for i in range(num_samples))
        super().__init__(paths = lines, circular = True, *args, **kwargs)
