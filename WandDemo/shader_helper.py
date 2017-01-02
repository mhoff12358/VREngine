import scene_system as sc

class ShaderHelper(object):
    force_pixel_shader_to_normals = False

    @classmethod
    def Default(cls, vertex_type_name = "all", include_world_coords = True, pixel_shader_name = "ps_blue.cso"):
        if vertex_type_name == "all":
            vertex_type = sc.VertexType.all
        elif vertex_type_name == "texture":
            vertex_type = sc.VertexType.texture
        elif vertex_type_name == "normal":
            vertex_type = sc.VertexType.normal
        elif vertex_type_name == "location":
            vertex_type = sc.VertexType.location
        vertex_shader_name = "vs_{}_apply_mvp{}.cso".format(vertex_type_name, "_world" if include_world_coords else "")
        return sc.ShaderDetails(
            sc.VectorShaderIdentifier((
                sc.ShaderIdentifier(vertex_shader_name, sc.ShaderStage.Vertex(), vertex_type),
                sc.ShaderIdentifier("ps_normals.cso" if cls.force_pixel_shader_to_normals else pixel_shader_name, sc.ShaderStage.Pixel()))))
