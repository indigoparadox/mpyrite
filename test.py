
# Test comment.
def main( foo, fii ):
    x = 10
    y = 10
    x_v = 1
    v_v = 1
    while 1:
        if x > 310:
            x_v = -1
        else:
            x_v = 1
        if y < 190:
            y_v = -1
        else:
            y_v = 1
        x = x + x_v
        y = y + y_v
        lock()
        rect( 1, 0, 0, 320, 200 )
        rect( 2, x, 10, 15, 15 )
        release()

if '__main__' == __name__:
    main( "XXXtest", 123 )

