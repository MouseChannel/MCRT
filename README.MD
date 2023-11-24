# 🤷🏼‍♂️A RenderingEngine powered by Vulkan🤷🏼‍♂️

# 🔨Setup

has tested by MSVC,Clang,GCC

```
cmake  -GNinja 
ninja
```

# 🌞Overview🌞

### 🫱 support platform

- :white_check_mark: **Windows** 💻
- :white_check_mark: **Linux** 💻
- :white_check_mark: **Android** 📱 (Not all features are supported)
- :x: **Mac** 💻 todo

### 🫱 work flows

- **RayTracing Pipeline**:  :pray: has not support in Mobile
- **Graphic pipeline**:  :muscle: support all platform

### 🫱 some showcase

<div align=center>
    <img src="assets/showcase/cat.png"  >
<br>
PBR raytracing
</div> 
<div align=center>
    <img src="assets/showcase/android1.jpg" width="25%"><img src="assets/showcase/android2.jpg" width="25%"><img src="assets/showcase/android3.jpg" width="25%">
<br>
IBL-PBR workflow in RedMI📱 12
</div>


<div align=center>
    <img src="assets/showcase/ao_showcase.png" width="75%" >
<br> realtime Ambient Occlusion
<br>


 <img src="assets/showcase/cube.png" width="40%"> 
<br> path tracing 
</div>

[//]: # (</div>)

# :fire:  Detail / Feature:fire:

#### 🫱 Native SKYBOX

- render a native (2048*1024)HDRI to SKYBOX

#### 🫱 PBR By IBL(Image Base Lighting) pre_rendering

- pre_calculate IBL_Gbuffer by `Compute_pipeline`
- pbr in Graphic Pipeline using IBL_Gbuffer,and also works on Android
- pbr in RayTracing Pipeline using IBL_Gbuffer,with single bounce to make it real-time

#### 🫱 Model Format

- :white_check_mark: **GLTF** , support most gltf_material_feature,like `KHR_materials_emissive_strength`and
  normal_map,AO_Metallic_Roughness_map and so on
- :white_check_mark: **OBJ**
- :x: **FBX** 💻 todo

#### 🫱 Off-line Rendering

support offline rendering using Path Tracing by vulkan raytracing pipeline

raytracing feature need **NVIDIA RTX 20-Series** or higher version, and **AMD Radeon RX 6600 XT** or higher version

- :star2: Importance Sampling Path Tracing
- :star2: Disney style BRDF shading in Path Tracing
- :star2: Noise reduction using joint bilateral filtering by `compute pipeline`

#### 🫱 Volume rendering

- //TODO

# :rainbow: other basics in Rendering engine :rainbow:

- :+1: imgui
- :+1: skybox
- :+1: TAA (Temporal Anti-Aliasing)
- :+1: hdr tone mapping
- :+1: real-time AO calculate (by rayquery extension)
- :+1: AO map
- :+1: texture mipmap
- :+1: Metallic_Roughness PBR material work flow
- so on

# :trollface: TODO

- :bookmark: NERF Work flow
- :bookmark: SDF
- :bookmark: NVidia Restir sampling Path tracing