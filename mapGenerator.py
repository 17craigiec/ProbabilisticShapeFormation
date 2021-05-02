import tkinter as tk
import math

window = tk.Tk(className='Map Generation Tool: Click Around')
myCanvas = tk.Canvas(window)
myCanvas.configure(width=500, height=500)
myCanvas.pack()

d_per_pixel = 400.0/500
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
    points.append((x, y))

    if len(points) > 1:
      myCanvas.create_line(points[-1][0], points[-1][1], points[-2][0], points[-2][1])

    print("Point Added: ("+str(x*d_per_pixel)+","+str(y*d_per_pixel)+")")

def calc_distance(p1, p2):
    dx = p2[0]*d_per_pixel-p1[0]*d_per_pixel
    dy = p2[1]*d_per_pixel-p1[1]*d_per_pixel
    return pow(pow(dx,2) +  pow(dy,2), 0.5)

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

    p_str = "{ "

    num_pts = len(points)
    for i in range(num_pts):
        p_str += "." + str(i) + " = { "
        for j in range(num_pts):
            if i != j:
                p_str += "." + str(j) + " = { "
                p_str += "." + "{:.2f}".format(calc_distance(points[i], points[j]), 3) + " = " + "{:.2f}".format(math.atan2(points[j][0]-points[i][0],points[j][1]-points[i][1]),3)
                p_str += "}"
                if i == num_pts-1:
                    if j < num_pts-2:
                        p_str += ", "
                else:
                    if j != num_pts-1:
                        p_str += ", "
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
