(load "alcaroite.scm")


(define cornell-white
  (material "matte" "reflectance" (spectrum-linear
    0.343 0.445 0.551 0.624 0.665 0.687 0.708 0.723 0.715 0.710 0.745
    0.758 0.739 0.767 0.777 0.765 0.751 0.745 0.748 0.729 0.745 0.757
    0.753 0.750 0.746 0.747 0.735 0.732 0.739 0.734 0.725 0.721 0.733
    0.725 0.732 0.743 0.744 0.748 0.728 0.716 0.733 0.726 0.713 0.740
    0.754 0.764 0.752 0.736 0.734 0.741 0.740 0.732 0.745 0.755 0.751
    0.744 0.731 0.733 0.744 0.731 0.712 0.708 0.729 0.730 0.727 0.707
    0.703 0.729 0.750 0.760 0.751 0.739 0.724 0.730 0.740 0.737
    )))

(define cornell-green
  (material "matte" "reflectance" (spectrum-linear
    0.092 0.096 0.098 0.097 0.098 0.095 0.095 0.097 0.095 0.094 0.097
    0.098 0.096 0.101 0.103 0.104 0.107 0.109 0.112 0.115 0.125 0.140
    0.160 0.187 0.229 0.285 0.343 0.390 0.435 0.464 0.472 0.476 0.481
    0.462 0.447 0.441 0.426 0.406 0.373 0.347 0.337 0.314 0.285 0.277
    0.266 0.250 0.230 0.207 0.186 0.171 0.160 0.148 0.141 0.136 0.130
    0.126 0.123 0.121 0.122 0.119 0.114 0.115 0.117 0.117 0.118 0.120
    0.122 0.128 0.132 0.139 0.144 0.146 0.150 0.152 0.157 0.159
    )))

(define cornell-red
  (material "matte" "reflectance" (spectrum-linear
    0.040 0.046 0.048 0.053 0.049 0.050 0.053 0.055 0.057 0.056 0.059
    0.057 0.061 0.061 0.060 0.062 0.062 0.062 0.061 0.062 0.060 0.059
    0.057 0.058 0.058 0.058 0.056 0.055 0.056 0.059 0.057 0.055 0.059
    0.059 0.058 0.059 0.061 0.061 0.063 0.063 0.067 0.068 0.072 0.080
    0.090 0.099 0.124 0.154 0.192 0.255 0.287 0.349 0.402 0.443 0.487
    0.513 0.558 0.584 0.620 0.606 0.609 0.651 0.612 0.610 0.650 0.638
    0.627 0.620 0.630 0.628 0.642 0.639 0.657 0.639 0.635 0.642
    )))

(define cornell-light
  (material "matte"
    "reflectance" (spectrum .78)
    "emittance" (spectrum-linear 0.0 8.0 15.6 18.4)))

(define mirror
  (material "perfect-mirror" "reflectance" (spectrum 1)))


(define (cornell-floor . material)
  (object
    "shape" (shape "quadmesh"
              "vertices" (vec-list
                           .5528  0 0
                           0      0 0
                           0      0 .5592
                           .5496  0 .5592)
              "faces" (int-list 0 1 2 3))
    "material" (if (null? material) cornell-white (car material))
    "priority" 10
    "transform" (identity)))


(define (cornell-ceiling . material)
  (object
    "shape" (shape "quadmesh"
              "vertices" (vec-list
                           .5560 .5488 .0000
                           .5560 .5488 .5592
                           .0000 .5488 .5592
                           .0000 .5488 .0000
                           .3430 .5488 .2270
                           .3430 .5488 .3320
                           .2130 .5488 .3320
                           .2130 .5488 .2270
                           )
              "faces" (int-list 0 1 2 3   7 6 5 4))
    "material" (if (null? material) cornell-white (car material))
    "priority" 10
    "transform" (identity)))


(define (cornell-light-mesh . material)
  (object
    "shape" (shape "quadmesh"
              "vertices" (vec-list
                            0.343 0.5487 0.227
                            0.343 0.5487 0.332
                            0.213 0.5487 0.332
                            0.213 0.5487 0.227
                            )
              "faces" (int-list 0 1 2 3))
    "material" (if (null? material) cornell-light (car material))
    "priority" 10
    "transform" (identity)))


(define (cornell-back-wall . material)
  (object
    "shape" (shape "quadmesh"
              "vertices" (vec-list
                           .5496  0     .5592
                           0      0     .5592
                           0      .5488 .5592
                           .5560  .5488 .5592)
              "faces" (int-list 0 1 2 3))
    "material" (if (null? material) cornell-white (car material))
    "priority" 10
    "transform" (identity)))


(define (cornell-right-wall . material)
  (object
    "shape" (shape "quadmesh"
              "vertices" (vec-list
                           0  0     .5592
                           0  0     0
                           0  .5488 0
                           0  .5488 .5592)
              "faces" (int-list 0 1 2 3))
    "material" (if (null? material) cornell-green (car material))
    "priority" 10
    "transform" (identity)))


(define (cornell-left-wall . material)
  (object
    "shape" (shape "quadmesh"
              "vertices" (vec-list
                           .5528  0     0
                           .5496  0     .5592
                           .5560  .5488 .5592
                           .5560  .5488 0)
              "faces" (int-list 0 1 2 3))
    "material" (if (null? material) cornell-red (car material))
    "priority" 10
    "transform" (identity)))

(define (cornell-short-block . material)
  (object
    "shape" (shape "quadmesh"
              "vertices" (vec-list
                            0.130 0.165 0.065
                            0.082 0.165 0.225
                            0.240 0.165 0.272
                            0.290 0.165 0.114

                            0.290 0.000 0.114
                            0.290 0.165 0.114
                            0.240 0.165 0.272
                            0.240 0.000 0.272

                            0.130 0.000 0.065
                            0.130 0.165 0.065
                            0.290 0.165 0.114
                            0.290 0.000 0.114

                            0.082 0.000 0.225
                            0.082 0.165 0.225
                            0.130 0.165 0.065
                            0.130 0.000 0.065

                            0.240 0.000 0.272
                            0.240 0.165 0.272
                            0.082 0.165 0.225
                            0.082 0.000 0.225)
              "faces" (int-list
                        0 1 2 3
                        4 5 6 7
                        8 9 10 11
                        12 13 14 15
                        16 17 18 19))
    "material" (if (null? material) cornell-white (car material))
    "priority" 10
    "transform" (identity)))


(define (cornell-tall-block . material)
  (object
    "shape" (shape "quadmesh"
              "vertices" (vec-list
                            0.423 0.330 0.247
                            0.265 0.330 0.296
                            0.314 0.330 0.456
                            0.472 0.330 0.406

                            0.423 0.000 0.247
                            0.423 0.330 0.247
                            0.472 0.330 0.406
                            0.472 0.000 0.406

                            0.472 0.000 0.406
                            0.472 0.330 0.406
                            0.314 0.330 0.456
                            0.314 0.000 0.456

                            0.314 0.000 0.456
                            0.314 0.330 0.456
                            0.265 0.330 0.296
                            0.265 0.000 0.296

                            0.265 0.000 0.296
                            0.265 0.330 0.296
                            0.423 0.330 0.247
                            0.423 0.000 0.247
                            )
              "faces" (int-list
                        0 1 2 3
                        4 5 6 7
                        8 9 10 11
                        12 13 14 15
                        16 17 18 19))
    "material" (if (null? material) cornell-white (car material))
    "priority" 10
    "transform" (identity)))

(define (cornell-skybox)
  (skybox "solid"
    "radiance" (spectrum 0.0)))


(define (cornell-camera)
  (camera "pinhole"
    "fov" 39.3076481
    "transform" (look-at
                  (vec .278 .273 -.800)
                  (vec .278 .273 1)
                  (vec 0 1 0))))


(define (cornell-box-empty)
  (cornell-skybox)
  (cornell-camera)
  (cornell-ceiling)
  (cornell-light-mesh)
  (cornell-floor)
  (cornell-back-wall)
  (cornell-left-wall)
  (cornell-right-wall))

(define (cornell-box)
  (cornell-box-empty)
  (cornell-short-block)
  (cornell-tall-block))


