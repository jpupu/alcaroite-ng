(load "linalg.scm")

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
  (cond ((and (list? v) (list? (car v)))
         (display (string-append
                    "mat "
                    (matrix->string v))))
        ((list? v)
         (display (string-append "vec " (vector->string v))))
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


