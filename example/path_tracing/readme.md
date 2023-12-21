```
    simple path tracing
    shade(p)
        if ray hit the light
            return light_emit * BRDF * cosine / pdf
        else if ray hit an object at q
            return shade(q) * BRDF * cosine / pdf
  
```

![path_tracing](../../assets/showcase/cube.png "👍👍")



<div align=center>
     <img src="../../assets/showcase/cook_torrance_path_tracing.png"  width="50%" ><img src="../../assets/showcase/path_tracing.gif"  width="50%" >
</div>
 