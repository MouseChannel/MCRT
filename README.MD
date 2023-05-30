# ray_tracing by Vulkan

### setup

```
cmake  -GNinja 
ninja
```

current now can only be compiled by **clang**


## What achieved?

- raster, using `Graphic Pipeline`
- **path tracing**, using   `Ray Tracing Pipeline `
- **AO**, using `Ray Query`extension
- Denoisy, using `Compute Pipeline`
- Some classical Material
  - blinn phong
  - cook torrance BRDF and together with some disney principle BRDF

## example

### blinn_phone

[Blinn_phong_example](https://github.com/MouseChannel/MCRT/tree/master/example/ray_tracing_blinn_phong "👍")

---



### path_tracing

[path_tracing_example](https://github.com/MouseChannel/MCRT/tree/master/example/path_tracing "👍")

simple path_tracing and denoisy

---



### cook torrance BRDF path_tracing

just replace **lambert diffuse** to **cook torrance**

[cook_torrance](https://github.com/MouseChannel/MCRT/tree/master/example/cook_torrance)

---