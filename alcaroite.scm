(load "linalg.scm")

(define spectrum-samples 16)

(define (vec-list . vals)
  (cons vec-list vals))

(define (int-list . vals)
  (cons int-list vals))

(define (lerp x a b)
  (+ (* (- 1 x) a) (* x b)))

(define (lerp-lookup-indexed i-frac vals)
  (cond ((= (length vals) 1)
          (car vals))
        ((<= i-frac 1)
          (lerp i-frac (car vals) (cadr vals)))
        (else
          (lerp-lookup-indexed (- i-frac 1) (cdr vals)))))

(define (lerp-lookup x vals)
  (let* ((max-index (- (length vals) 1))
         (i-frac (* x max-index)))
    (lerp-lookup-indexed i-frac vals)))

(define (flatten x)
  (cond ((null? x) '())
        ((list? x) (append (flatten (car x)) (flatten (cdr x))))
        (else (list x))))

(define (matrix->string m)
  (foldr
    (lambda (acc x)
      (string-append
        acc
        (number->string x)
        " "))
    ""
    (flatten (transpose (drop-row m 3)))))

(define (vector->string v)
  (foldr (lambda (acc x) (string-append acc (number->string x) " ")) "" v))

(define (display-value v)
  (cond ((and (list? v) (eq? (car v) vec-list))
         (display (string-append
                    "veclist "
                    (number->string (/ (- (length v) 1) 3))
                    " "
                    (vector->string (cdr v)))))
        ((and (list? v) (eq? (car v) int-list))
         (display (string-append
                    "intlist "
                    (number->string (- (length v) 1))
                    " "
                    (vector->string (cdr v)))))
        ((and (list? v) (list? (car v)))
         (display (string-append
                    "mat "
                    (matrix->string v))))
        ((and (list? v) (= 3 (length v)))
         (display (string-append "vec " (vector->string v))))
        ((and (list? v) (= spectrum-samples (length v)))
         (display (string-append "spectrum " (vector->string v))))
        (else
          (display v))))

(define display-values
  (lambda (args)
    (if (null? args)
      (begin
        (display ".\n\n"))
      (begin
        (display ":: ")
        (display (car args))
        (display " ")
        (display-value (cadr args))
        (display "\n")
        (display-values (cddr args))))))

(define (vec x y z) (list x y z))

(define (spectrum . vals)
  (cond ((= (length vals) spectrum-samples)
         vals)
        ((= (length vals) 1)
         (duplicate-list (car vals) spectrum-samples))
        (else
          (error "Bad number of spectrum samples" vals))))

(define (spectrum-linear . vals)
  (map (lambda (i) (lerp-lookup (/ i (- spectrum-samples 1)) vals))
    (range 0 spectrum-samples)))


(define blocks '())
(define (block type . values)
  (let ((name (string-append "_" type (number->string (length blocks)))))
    (set! blocks (append blocks (list (list type name values))))
    name))

(define (material material-type . values)
  (apply block (append (list "material" "type" material-type) values)))

(define (shape shape-type . values)
  (apply block (append (list "shape" "type" shape-type) values)))

(define (skybox skybox-type . values)
  (apply block (append (list "skybox" "type" skybox-type) values)))

(define (camera camera-type . values)
  (apply block (append (list "camera" "type" camera-type) values)))

(define (object . values)
  (apply block (append '("object") values)))

(define (display-block block)
  (let
    ((type (car block))
     (name (cadr block))
     (values (cddr block)))
    (display type)
    (display " ")
    (display name)
    (newline)
    (apply display-values values)))

(define (finish) (for-each display-block blocks))

