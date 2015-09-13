
(define pi 3.14159265358979)

(define (vec-op op v a)
  (if (null? v) '()
    (if (list? a)
      (cons (op (car v) (car a)) (vec-op op (cdr v) (cdr a)))
      (cons (op (car v) a) (vec-op op (cdr v) a)))))

(define (vec+ v a) (vec-op + v a))
(define (vec- v a) (vec-op - v a))
(define (vec* v a) (vec-op * v a))
(define (vec/ v a) (vec-op / v a))

(define (neg xs)
  (if (null? xs) '()
    (cons (- 0 (car xs)) (neg (cdr xs)))))

(define (dot a b)
  (if (null? a)
    0
    (+ (* (car a) (car b)) (dot (cdr a) (cdr b)))))

(define (cross u v)
  (let ((u1 (car u))
        (u2 (cadr u))
        (u3 (caddr u))
        (v1 (car v))
        (v2 (cadr v))
        (v3 (caddr v)))
    (list
      (- (* u2 v3) (* u3 v2))
      (- (* u3 v1) (* u1 v3))
      (- (* u1 v2) (* u2 v1)))))


(define (norm v)
  (sqrt (dot v v)))

(define (normalize v)
  (vec/ v (norm v)))

(define (pow x i)
  (if (= i 0) 1
    (* x (pow x (- i 1)))))

(define (matrix
          m00 m10 m20 m30
          m01 m11 m21 m31
          m02 m12 m22 m32
          m03 m13 m23 m33)
  (list
    (list m00 m01 m02 m03)
    (list m10 m11 m12 m13)
    (list m20 m21 m22 m23)
    (list m30 m31 m32 m33)))


(define (transpose m)
  (apply map list m))

(define (column m i)
  (if (= i 0)
    (car m)
    (column (cdr m) (- i 1))))

(define (row m i)
  (column (transpose m) i))

(define (duplicate-list xs n)
  (if (= n 1) (list xs)
    (append (list xs) (duplicate-list xs (- n 1)))))

(define (duplicate-elements xs n)
  (map
    (lambda (x)
      (apply append (duplicate-list (list x) n)))
    xs))

(define (matrix-multiply a b)
  (transpose
  (map
    (lambda (a-row)
      (map dot
        (duplicate-list a-row 4)
        b))
    (transpose a))))

(define (roll-left m n)
  (if (= n 0) m
    (let ((m-rolled (append (cdr m) (list (car m)))))
      (roll-left m-rolled (- n 1)))))

(define (roll-right m n)
  (roll-left m (- (length m) n)))

(define (roll-up m n)
  (transpose (roll-left (transpose m) n)))

(define (roll-down m n)
  (transpose (roll-right (transpose m) n)))

(define (drop-column m i)
  (roll-right (cdr (roll-left m i)) i))

(define (drop-row m i)
  (transpose (drop-column (transpose m) i)))

(define (minor m i j)
  (determinant (drop-column (drop-row m j) i)))

(define (cofactor m i j)
  (* (pow -1 (+ i j)) (minor m i j)))

(define (range a b)
  (if (>= a b) '()
    (cons a (range (+ a 1) b))))

(define (sum xs)
  (apply + xs))

(define (determinant m)
  (if (= (length m) 2)
    (- (* (caar m) (cadadr m)) (* (caadr m) (cadar m)))
    (sum
      (map
        (lambda (i)
          (* (pow -1 i) (car (column m i)) (minor m i 0)))
        (range 0 (length m))))))

(define (map-matrix func m)
  (let ((column-indices (range 0 (length m)))
        (row-indices (range 0 (length (car m)))))
    (map (lambda (col i)
           (map (lambda (elem j) (func i j elem))
             col row-indices))
           m column-indices)))

(define (matrix-of-cofactors m)
  (map-matrix (lambda (i j x) (cofactor m i j)) m))

(define (inverse m)
  (let ((det (determinant m)))
    (map-matrix
      (lambda (i j x) (/ x det))
      (transpose (matrix-of-cofactors m)))))

(define (display-matrix m)
  (apply map
    (lambda xs (display xs) (newline))
    m)
  (newline))

(define (identity)
  (matrix
    1 0 0 0
    0 1 0 0
    0 0 1 0
    0 0 0 1))

(define (translate x y z)
  (matrix
    1 0 0 x
    0 1 0 y
    0 0 1 z
    0 0 0 1))

(define (rotate-x degrees)
  (let* ((radians (* (/ degrees 180.0) pi))
         (s (sin radians))
         (-s (- 0 s))
         (c (cos radians)))
    (matrix
      1   0   0   0
      0   c   -s  0
      0   s   c   0
      0   0   0   1)))

(define (rotate-y degrees)
  (let* ((radians (* (/ degrees 180.0) pi))
         (s (sin radians))
         (-s (- 0 s))
         (c (cos radians)))
    (matrix
      c   0   s   0
      0   1   0   0
      -s  0   c   0
      0   0   0   1)))

(define (rotate-z degrees)
  (let* ((radians (* (/ degrees 180.0) pi))
         (s (sin radians))
         (-s (- 0 s))
         (c (cos radians)))
    (matrix
      c  -s   0   0
      s   c   0   0
      0   0   1   0
      0   0   0   1)))

(define (rotate axis degrees)
  (let* ((radians (* (/ degrees 180.0) pi))
         (s (sin radians))
         (c (cos radians))
         (1-c (- 1 c))
         (u (normalize axis))
         (x (car u))
         (y (cadr u))
         (z (caddr u)))
    (matrix
      (+ c (* ux ux 1-c))
      (- (* x y 1-c) (* z s))
      (+ (* x z 1-c) (* y s))
      0
      (+ (* y x 1-c) (* z s))
      (+ c (* y y 1-c))
      (- (* y z 1-c) (* x s))
      0
      (- (* z x 1-c) (* y s))
      (+ (* z y 1-c) (* x s))
      (+ c (* z z 1-c))
      0
      0 0 0 1)))

(define (scale-x f)
  (matrix
    f 0 0 0
    0 1 0 0
    0 0 1 0
    0 0 0 1))

(define (scale-y f)
  (matrix
    1 0 0 0
    0 f 0 0
    0 0 1 0
    0 0 0 1))

(define (scale-z f)
  (matrix
    1 0 0 0
    0 1 0 0
    0 0 f 0
    0 0 0 1))

(define (scale f)
  (matrix
    f 0 0 0
    0 f 0 0
    0 0 f 0
    0 0 0 1))

(define (look-at eye center up)
  (let* ((f (normalize (vec- center eye)))
         (s (cross f (normalize up)))
         (u (cross (normalize s) f))
         (f0 (append f '(0)))
         (s0 (append s '(0)))
         (u0 (append u '(0)))
         (M (list s0 u0 (neg f0) (list 0 0 0 1)))
         (T (apply translate eye)))
    (matrix-multiply T M)))





