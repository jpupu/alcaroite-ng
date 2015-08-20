(load "alcaroite.scm")

(object
  "shape" (shape "sphere")
  "material" (material "glass" "ior" 1.5 "absorbance" (vec 4 2 9))
  "priority" 20
  "transform" (identity)
  "inverse_transform" (identity))

(object
  "shape" (shape "plane")
  "material" (material "matte" "reflectance" .8)
  "priority" 40
  "transform" (translate 0 -1 0)
  "inverse_transform" (inverse (translate 0 -1 0)))

(skybox "fancy")

(finish)

