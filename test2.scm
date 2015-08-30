(load "alcaroite.scm")

(object
  "shape" (shape "sphere")
  "material" (material "glass" "ior" (spectrum-linear 1.53 1.50) "absorbance" (spectrum-linear 0 0 1))
  "priority" 20
  "transform" (identity))

(object
  "shape" (shape "plane")
  "material" (material "matte" "reflectance" (spectrum .8))
  "priority" 40
  "transform" (translate 0 -1 0))

(skybox "fancy")

(camera "pinhole"
  "fov" 90
  "transform" (translate 0 .3 2))

(finish)

