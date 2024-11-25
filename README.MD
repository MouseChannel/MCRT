<!-- <div align="center">
<img width="200" src="./assets/showcase/logo.png"/> -->

# 🤷🏼‍♂️  MC Rendering-RunTime 🤷🏼‍♂️

[![ Window ](https://github.com/MouseChannel/MCRT/actions/workflows/windows.yaml/badge.svg)](https://github.com/MouseChannel/MCRT/actions/workflows/windows.yaml?query=branch%3Arelease)
[![ Linux ](https://github.com/MouseChannel/MCRT/actions/workflows/linux.yaml/badge.svg)](https://github.com/MouseChannel/MCRT/actions/workflows/linux.yaml?query=branch%3Arelease)
[![ Android ](https://github.com/MouseChannel/MCRT/actions/workflows/android.yaml/badge.svg)](https://github.com/MouseChannel/MCRT/actions/workflows/android.yaml?query=branch%3Arelease)

</div>

<!-- # 🔨Setup

has tested by MSVC,Clang,GCC

```
cmake  -GNinja 
ninja
``` -->

# 🌞Overview🌞

### 🫱 support platform

- ✅ **Windows** 💻
- ✅ **Linux** 💻
- ✅ **Android** 📱 (Not all features are supported)
- ❌ **Mac** 💻 todo 

### 🫱 work flows

- **RayTracing Pipeline**:  🙏 has not support in Mobile
- **Graphic pipeline**:  💪 support all platform

### 🫱 some showcase

<div align=center>
    <img src="assets/showcase/cat1.png"  width="30%" ><img src="assets/showcase/cat2.png"  width="30%" ><img src="assets/showcase/cat3.png"  width="30%" >
<br>
IBL-PBR raytracing
</div> 
<div align=center>
    <img src="assets/showcase/android1.jpg" width="25%"><img src="assets/showcase/android2.jpg" width="25%"><img src="assets/showcase/android3.jpg" width="25%">
<br>
IBL-PBR workflow in RedMI📱 12
<br>
</div>

<div align=center >
    <img src="assets/showcase/ao_showcase.png" width="75%" >
<br> realtime Ambient Occlusion
<br>
<a href="./assets/showcase/cloudDemo.webm">
 <img src="assets/showcase/cloud.gif" width="43%"> 
</a>
 <img src="assets/showcase/cube.png"  width="37%"> 
<br>
 Volumetric cloud 👈👈-----------------👉👉 Path tracing 
</div>

# 🔥 Detail / Feature🔥

### 🫱 Native SKYBOX

- render a native (2048*1024)HDRI to SKYBOX

### 🫱 PBR By IBL(Image Base Lighting) pre_rendering

- pre_calculate IBL_Gbuffer by `Compute_pipeline`
- pbr in Graphic Pipeline using IBL_Gbuffer,and also works on Android
- pbr in RayTracing Pipeline using IBL_Gbuffer,with single bounce to make it real-time

### 🫱 Vulkan CUDA interOP

compute-heavy task can be handled by **Computer-Shader** or **CUDA**

- Vulkan-CUDA Memory share and Image_data share,sync by Semaphore

 

### 🫱  Volumetric cloud rendering

raymarching cloud density from 3D perlin noise.

Use vulkan raytracing pipeline to raymarching,
so [A supported GPU and Driver](https://www.khronos.org/blog/ray-tracing-in-vulkan#blog_Call_to_Action) is necessary.

- 🌟 pre_compute 3D(256x256x256) Perlin-Noise by compute shader, and sampling by boundary mirror uv to make it continuous
- 🌟 raymarching in **rmiss** shader-stage, use `ray-max-range` in `traceRayEXT` to simulate one step in raymarching

### 🫱 Off-line Rendering

support offline rendering using Path Tracing by vulkan raytracing pipeline

also [A supported GPU and Driver](https://www.khronos.org/blog/ray-tracing-in-vulkan#blog_Call_to_Action) is needed,
like **NVIDIA RTX 20-Series** or higher version, and **AMD Radeon RX 6600 XT** or higher version

- 🌟 Importance Sampling Path Tracing
- 🌟 Disney style BRDF shading in Path Tracing
- 🌟 Noise reduction using joint bilateral filtering by `compute pipeline`
 