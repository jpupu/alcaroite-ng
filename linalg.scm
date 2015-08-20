

(define (dot a b)
  (if (null? a)
    0
    (+ (* (car a) (car b)) (dot (cdr a) (cdr b)))))

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


