package Kinect.AngleGenerator;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author zack
 */
public class Vector {
    public double x;
    public double y;
    public double z;
    public Vector(double x, double y, double z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }
    public Vector(Point p1, Point p2)
    {
        this.x = p2.x - p1.x;
        this.y = p2.y - p1.y;
        this.z = p2.z - p1.z;
    }
    public void printVector()
    {
        System.out.println("("+this.x+", "+this.y+", "+this.z+")\n");
    }
    public static Vector add(Vector v1, Vector v2)
    {
        Vector result = new Vector(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
        return result;
    }
    public static Vector subtract(Vector v1, Vector v2)
    {
        Vector result = new Vector(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
        return result;
    }
    public double magnitude()
    {
        return Math.sqrt(Math.pow(this.x,2)+Math.pow(this.y,2)+Math.pow(this.z,2));
    }
    public double dotProduct(Vector v)
    {
        return this.x*v.x+this.y*v.y+this.z*v.z;
    }
    public Vector crossProduct(Vector v)
    {
        return new Vector(this.y*v.z-this.z*v.y, this.z*v.x-this.x*v.z, this.x*v.y-this.y*v.x);
    }
    public double angle(Vector v)
    {
        return 180/Math.PI*Math.acos(this.dotProduct(v)/(this.magnitude()*v.magnitude()));
    }
}
