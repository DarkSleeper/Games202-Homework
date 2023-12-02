*代码只展示作业中需要完成的部分

# 作业一
## result
|  shadow_map  | PCF | PCSS |
|  ----  | ---- | ---- |
|  <img src="https://github.com/DarkSleeper/Games202-Homework/assets/48831197/ead3ddaa-4d3e-4f0d-a000-f4b138b91781" width=300>  | <img src="https://github.com/DarkSleeper/Games202-Homework/assets/48831197/26366f17-3fe1-473f-8078-c2858c9e8237" width=300> | <img src="https://github.com/DarkSleeper/Games202-Homework/assets/48831197/d77d4555-0cf4-4c56-9a72-3a5e1ca4fe7d" width=300> |

### PCSS issues
![image](https://github.com/DarkSleeper/Games202-Homework/assets/48831197/eaef7a62-5f12-4999-bd99-570ba5354f88)

# 作业三
需要注意的是，代码中要判断ssr raycast到的点的法线与着色点法线的关系<br>
|  cube0  | cube1 | cave |
|  ----  | ---- | ---- |
| <img src="https://github.com/DarkSleeper/Games202-Homework/assets/48831197/94d4cd0b-ed42-437d-92ce-39eea68a858c" width=256> | <img src="https://github.com/DarkSleeper/Games202-Homework/assets/48831197/ceda6dbd-1c9e-4738-a545-2d749f5a07bc" width=256><br>  | <img src="https://github.com/DarkSleeper/Games202-Homework/assets/48831197/136405e2-23c1-45eb-8ac5-47362d89374d" width=380> |

# 作业四
代码中手写微表面模型的DFG项；<br>
对于Eavg，因为和出射方向无关，所以重要性采样的avg算法和均一采样的一致；<br>
因为表面normal的法线为(0,0,1)，所以转换坐标系时无需转到物体表面的tangent空间；<br>
<img src="https://github.com/DarkSleeper/Games202-Homework/assets/48831197/1b521e0f-74b3-44d7-9133-7fa4183d9cfd" width=512>

# 作业五
需要注意采样点是否超出屏幕、物体id匹配、计算不应产生nan结果

