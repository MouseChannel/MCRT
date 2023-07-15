```
blinn_phone

radiance = ambient + diffuse + specular


ambient = 0.05 * color

diffuse = irradiance * color
	= light_intensity / pow(light_distance,2) * cosine + color


specular = pow(dot(half_dir,view_dir),32	)
```

![blinn_phong](../../assets/blinn_phong.png "😊😊😊")
