import tkinter as tk
import math

window = tk.Tk(className='Map Generation Tool: Click Around')
myCanvas = tk.Canvas(window)
myCanvas.configure(width=500, height=500)
myCanvas.pack()

d_per_pixel = 400.0/500
robot_radius = 5;
points = []

def create_circle(x, y, r, canvasName): #center coordinates, radius
    x0 = x - r
    y0 = y - r
    x1 = x + r
    y1 = y + r
    return canvasName.create_oval(x0, y0, x1, y1)

def m_click(eventorigin):

    global x,y
    x = eventorigin.x
    y = eventorigin.y
    r = 10
    create_circle(x, y, r, myCanvas)
    points.append((x*d_per_pixel, y*d_per_pixel))

    if len(points) > 1:
      myCanvas.create_line(points[-1][0]/d_per_pixel, points[-1][1]/d_per_pixel, points[-2][0]/d_per_pixel, points[-2][1]/d_per_pixel)

    print("Point Added: ("+str(x)+","+str(y)+")")

def calc_distance(p1, p2):
    dx = p2[0]-p1[0]
    dy = p2[1]-p1[1]
    return pow(pow(dx,2) +  pow(dy,2), 0.5)

def is_line_blocked(p1, p2, i, j):
    index = 0;
    for p in points:
        if p != p1 and p != p2:
            d = calc_distance(p1, p)
            blocking_angle = math.atan2(robot_radius, d)
            delta_angle = abs(math.atan2(p[1]-p1[1], p[0]-p1[0]) - math.atan2(p2[1]-p1[1], p2[0]-p1[0]));
            if delta_angle < blocking_angle and calc_distance(p1, p) < calc_distance(p1, p2):
                print("Point: "+str(i)+"   blocked from Point: "+str(j)+"   by Point: "+str(index))
                return True
        index = index + 1;
    return False



def rotate_points():
    theta = 3.14159/2
    count = 0
    for p in points:
        rot_x = p[0]*math.cos(theta)-p[1]*math.sin(theta)
        rot_y = p[0]*math.sin(theta)+p[1]*math.cos(theta)

        points[count] = (rot_x, rot_y)
        count += 1

def enter_pressed(event):
    # print the data structure the buzz way
    rotate_points()

    first_line_i = True
    p_str = "{ "

    num_pts = len(points)
    for i in range(num_pts):
        first_line_j = True
        p_str += "." + str(i) + " = { "
        for j in range(num_pts):
            if is_line_blocked(points[i], points[j], i, j):
                block_count = 0
            if i != j and not is_line_blocked(points[i], points[j], i, j):
                if not first_line_j:
                    p_str += ", "
                p_str += "." + str(j) + " = { "
                p_str += "." + "{:.2f}".format(calc_distance(points[i], points[j]), 3) + " = " + "{:.2f}".format(math.atan2(points[j][0]-points[i][0],points[j][1]-points[i][1]),3)
                p_str += "}"
                first_line_j = False
        first_line_i = False
        p_str += "}"
        if i != num_pts-1:
            p_str += ", "
    p_str += "}"
    print(p_str)
    print("================== ["+str(num_pts)+"] ROBOTS REQUIRED ==================")
    window.destroy()

window.geometry("500x500")
window.bind('<Button-1>', m_click)
window.bind('<Return>', enter_pressed)
window.mainloop()
