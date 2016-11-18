import scene_system as sc

class PathDraggableObject(sc.DelegatingActor):
	command_delegation = sc.DelegatingActor.GetDefaultDelegation()
	delegater = sc.delegate_for_command(command_delegation)

	def __init__(self, paths, circular = False, path_sample_rates = None, path_sample_rate = 5):
		super().__init__()
		
		if path_sample_rates is None:
			path_sample_rates = (path_sample_rate,) * len(paths)
		else:
			assert(len(path_sample_rates) == len(paths))

		self.current_location = sc.Location(0, 1, 0)
		self.controller_location_delta = sc.Location(0, 0, 0)
		self.radius = 0.1

		self.paths = paths
		self.circular = circular
		self.path_vertices = self.BuildPathVertices(path_sample_rates)

	def BuildPathVertices(self, path_sample_rates):
		path_vertices = [sc.ArrayFloat3(self.paths[0].at(0))]
		for path, sample_rate in zip(self.paths, path_sample_rates):
			for sample_fraction in range(0, sample_rate):
				vertex_location = path.at((sample_fraction + 1) / sample_rate)
				path_vertices.append(sc.ArrayFloat3(vertex_location))
		return [
			sc.ArrayFloat3((0, 0, 0)),
			sc.ArrayFloat3((0, 1, 0)),
			sc.ArrayFloat3((1, 0, 0)),
			]

	def PlaceSelf(self, latest_command):
		return self.scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.graphics_object_id),
			sc.PlaceComponent(
				"Sphere",
				sc.Pose(self.current_location, sc.Scale(self.radius))))
		
	@delegater(sc.CommandType.ADDED_TO_SCENE)
	def HandleAddToScene(self, args):
		latest_command = self.scene.FrontOfCommands()

		self.graphics_resources = sc.GraphicsResources.GetGraphicsResources(self.scene)

		self.path_id = self.scene.AddAndConstructGraphicsObject().id
		path_model_generator = sc.ModelGenerator(sc.VertexType.location, sc.D3DTopology.LINESTRIP)
		path_model_generator.AddVertexBatch(sc.Vertices(
			sc.VertexType.location,
			sc.VectorArrayFloat3(self.path_vertices)))
		path_model_generator.SetParts(sc.MapStringToModelSlice({"path":sc.ModelSlice(6, 0)}))
		path_model_generator.Finalize(
			self.graphics_resources.GetDeviceInterface(),
			self.graphics_resources.GetEntityHandler(),
			sc.ModelStorageDescription(False, True, False))
		self.path_model_name = sc.ResourceIdentifier.GetNewModelName("draggable_path")
		latest_command = self.scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.path_id),
			sc.CreateGraphicsObject(
				"basic",
				sc.VectorEntitySpecification((
					sc.EntitySpecification("path")
						.SetModel(sc.ModelDetails(
							sc.ModelIdentifier(self.path_model_name),
							path_model_generator))
						.SetShaders(sc.ShaderDetails(
							sc.VectorShaderIdentifier((
								sc.ShaderIdentifier("solidline.hlsl", sc.ShaderStage.Vertex(), sc.VertexType.location),
								sc.ShaderIdentifier("solidline.hlsl", sc.ShaderStage.Geometry()),
								sc.ShaderIdentifier("solidline.hlsl", sc.ShaderStage.Pixel()),
								))))
						.SetShaderSettingsValue(sc.ShaderSettingsValue((
							sc.VectorFloat((0.5, 0.5, 0.5, 1)),
							sc.VectorFloat((1.0,)))))
						.SetTextures(sc.VectorIndividualTextureDetails((sc.IndividualTextureDetails("terrain.png", sc.ShaderStages.All(), 0, 0),)))
						.SetComponent("path"),)),
				sc.VectorComponentInfo((sc.ComponentInfo("", "path"),))))
		latest_command = self.scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.path_id),
			sc.PlaceComponent(
				"path",
				sc.Pose(sc.Location(0, 0, 0))))

		self.graphics_object_id = self.scene.AddAndConstructGraphicsObject().id
		latest_command = self.scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.graphics_object_id),
			sc.CreateGraphicsObject(
				"basic",
				sc.VectorEntitySpecification((
					sc.EntitySpecification("sphere") \
						.SetModel(sc.ModelDetails(
							sc.ModelIdentifier("sphere.obj|Sphere")))
						.SetShaders(sc.ShaderDetails(
							sc.VectorShaderIdentifier((
								sc.ShaderIdentifier("vs_location_passthrough.cso", sc.ShaderStage.Vertex(), sc.VertexType.location),
								sc.ShaderIdentifier("ps_solidcolor.cso", sc.ShaderStage.Pixel()),
								))))
						.SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat((0.5, 0, 0)),)))
						.SetComponent("Sphere"),)),
				 sc.VectorComponentInfo((sc.ComponentInfo("", "Sphere"),))))
		latest_command = self.PlaceSelf(latest_command)
		latest_command = self.scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
			sc.AddGrabbableObject(self.id, sc.VectorCollisionShape((sc.CollisionShape(self.current_location, self.radius),))))

	@delegater(sc.HeadsetInterfaceCommand.LISTEN_CONTROLLER_MOVEMENT)
	def HandleControllerMovementWhileGrabbed(self, args):
		self.scene = self.GetScene()
		self.current_location = args.position.location + self.controller_location_delta
		self.PlaceSelf(self.scene.FrontOfCommands())

	@delegater(sc.GrabbableObjectCommand.OBJECT_GRABBED)
	def HandleGrabbed(self, args):
		if args.held:
			self.controller_location_delta = self.current_location - args.position.location
			self.scene.MakeCommandAfter(
				self.scene.FrontOfCommands(),
				sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
				sc.RemoveGrabbableObject(self.id))
		else:
			self.scene.MakeCommandAfter(
				self.scene.FrontOfCommands(),
				sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
				sc.AddGrabbableObject(self.id, sc.VectorCollisionShape((sc.CollisionShape(self.current_location, self.radius),))))
