# 🤷🏼‍♂️A RenderingEngine powered by Vulkan🤷🏼‍♂️

# 🔨Setup

has tested by MSVC,Clang,GCC

```
cmake  -GNinja 
ninja
```

# 🌞Overview🌞

### 🫱 support platform

- ✅ **Windows** 💻
- ✅ **Linux** 💻
- ✅ **Android** 📱 (Not all features are supported)
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

# 🔥  Detail / Feature🔥

#### 🫱 Native SKYBOX

- render a native (2048*1024)HDRI to SKYBOX

#### 🫱 PBR By IBL(Image Base Lighting) pre_rendering

- pre_calculate IBL_Gbuffer by `Compute_pipeline`
- pbr in Graphic Pipeline using IBL_Gbuffer,and also works on Android
- pbr in RayTracing Pipeline using IBL_Gbuffer,with single bounce to make it real-time

#### 🫱 Model Format

- ✅ **GLTF** , support most gltf_material_feature,like `KHR_materials_emissive_strength`and
  normal_map,AO_Metallic_Roughness_map and so on
- ✅ **OBJ**
- ❌ **FBX** 💻 todo

#### 🫱 Off-line Rendering

support offline rendering using Path Tracing by vulkan raytracing pipeline

raytracing feature need **NVIDIA RTX 20-Series** or higher version, and **AMD Radeon RX 6600 XT** or higher version

- 🌟 Importance Sampling Path Tracing
- 🌟 Disney style BRDF shading in Path Tracing
- 🌟 Noise reduction using joint bilateral filtering by `compute pipeline`

#### 🫱 Volume rendering

- //TODO

# ⛱️ other basics in Rendering engine ⛱️

- 👏 imgui
- 👏 skybox
- 👏 TAA (Temporal Anti-Aliasing)
- 👏 hdr tone mapping
- 👏 real-time AO calculate (by rayquery extension)
- 👏 AO map
- 👏 texture mipmap
- 👏 Metallic_Roughness PBR material work flow
- so on

# ♿ TODO

- 🔖 NERF Work flow
- 🔖 SDF
- 🔖 NVidia Restir sampling Path tracing