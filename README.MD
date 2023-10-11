# 🤷🏼‍♂️A RenderingEngine powered by Vulkan🤷🏼‍♂️

## 🔨Setup
has tested by MSVC,Clang,GCC
```
cmake  -GNinja 
ninja
```



## 🌞Overview🌞

### support platform

- :heavy_check_mark: **Windows** 💻
- :heavy_check_mark: **Linux** 💻
- :heavy_check_mark: **Android** 📱 (Not all features are supported)
- :x: **Mac** 💻 todo


### work flows
- **RayTracing Pipeline**:  :pray: has not support in Mobile
- **Graphic pipeline**:  :muscle: support all platform


### :fire:  Detail  /  Feature:fire:

#### PBR By IBL pre_rendering
- pre_calculate IBL_Gbuffer by `Compute_pipeline`
- pbr in Graphic Pipeline using IBL_Gbuffer,and also works on Android
- pbr in RayTracing Pipeline using IBL_Gbuffer,with single bounce to make it real-time

#### Model Format
- :heavy_check_mark: **GLTF** , support most gltf_material_feature,like `KHR_materials_emissive_strength`and normal_map,AO_Metallic_Roughness_map and so on
- :heavy_check_mark: **OBJ**
- :x: **FBX** 💻 todo

#### Off-line Rendering 
support offline rendering using Path Tracing by vulkan raytracing pipeline

raytracing feature need **NVIDIA RTX 20-Series** or higher version, and **AMD Radeon RX 6600 XT** or higher version
 - :star2: Importance Sampling Path Tracing  
 - :star2: Disney style BRDF shading in Path Tracing
 - :star2: Noise reduction using joint bilateral filtering by `compute pipeline` 

### :rainbow: other basics in Rendering engine 
- :+1: imgui
- :+1: skybox
- :+1: hdr
- :+1: real-time AO calculate (by rayquery extension)
- :+1: AO map
- :+1: mipmap
- :+1: Metallic_Roughness PBR material work flow
- so on


### :trollface: TODO
- :bookmark: SDF
- :bookmark: NVidia Restir sampling Path tracing