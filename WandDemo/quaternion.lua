-- Quaternions are just lists of four numbers

quaternion = {
AID_X = 1,
AID_Y = 2,
AID_Z = 4,
}

function quaternion.mult(q1, q2)
	return {
		q1[4]*q2[1] + q1[1]*q2[4] + q1[2]*q2[3] - q1[3]*q2[2],
		q1[4]*q2[2] - q1[1]*q2[3] + q1[2]*q2[4] + q1[3]*q2[1],
		q1[4]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1] + q1[3]*q2[4],
		q1[4]*q2[4] - q1[1]*q2[1] - q1[2]*q2[2] - q1[3]*q2[3],
	}
end

function quaternion.rotation_about_axis(axis_id, angle_in_radians)
	if axis_id == quaternion.AID_X then
		return { math.sin(angle_in_radians / 2), 0, 0, math.cos(angle_in_radians / 2) }
	elseif axis_id == quaternion.AID_Y then
		return { 0, math.sin(angle_in_radians / 2), 0, math.cos(angle_in_radians / 2) }
	elseif axis_id == quaternion.AID_Z then
		return { 0, 0, math.sin(angle_in_radians / 2), math.cos(angle_in_radians / 2) }
	else
		return nil
	end
end

function quaternion.invert(q)
	return { -q[1], -q[2], -q[3], q[4] }
end

function quaternion.apply(quat, vec)
	return quaternion.mult(quaternion.mult(quat, {vec[1], vec[2], vec[3], 0}), quaternion.invert(quat))
end

function quaternion.identity()
	return { 0, 0, 0, 1 }
end
