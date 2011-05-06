package Kinect.AngleGenerator;
/**
 *
 * @author zack ankuda
 * @editor chris orescan
 */
import java.util.*;
import java.io.*;

public class AngleGen {
    String sourceDir;
    String[] flist;
    Scanner fin;
    int findex;

    Point torso;
    Point neck;
    Point shoulder;
    Point elbow;
    Point wrist;
    Vector torsoToneck;
    Vector neckToshoulder;
    Vector shoulderToelbow;
    Vector elbowTowrist;
    Vector tricep_plane;
    int label = 0;

    int SKIPS = 0; //script leaves out SKIPS amount of lines for every line read

    public AngleGen(String dir) {
        sourceDir = dir;
        flist = new File(sourceDir).list();
        findex = 0;
    }
    public boolean hasNext(){
        return findex<flist.length;
    }
    public ArrayList<Integer[]> next() {
        if (flist[findex].startsWith("."))
            findex++;
        ArrayList<Integer[]> angles = new ArrayList<Integer[]>();
        try {
            fin = new Scanner(new File(sourceDir+"/"+flist[findex++]));
                System.out.println(flist[findex-1]);
            while (fin.hasNextDouble()) {
                int i = SKIPS;
                torso = new Point(fin.nextDouble(), fin.nextDouble(), fin.nextDouble());
                neck = new Point(fin.nextDouble(), fin.nextDouble(), fin.nextDouble());
                shoulder = new Point(fin.nextDouble(), fin.nextDouble(), fin.nextDouble());
                elbow = new Point(fin.nextDouble(), fin.nextDouble(), fin.nextDouble());
                wrist = new Point(fin.nextDouble(), fin.nextDouble(), fin.nextDouble());
                label = fin.nextInt();
                while (fin.hasNextLine() && i-- > 0) {
                    fin.nextLine();
                }
                torsoToneck = new Vector(torso, neck);
                neckToshoulder = new Vector(neck, shoulder);
                shoulderToelbow = new Vector(shoulder, elbow);
                tricep_plane = neckToshoulder.crossProduct(shoulderToelbow);
                elbowTowrist = new Vector(elbow, wrist);
                Integer[] angleSet = {(int) torsoToneck.angle(shoulderToelbow),
                                    (int) neckToshoulder.angle(shoulderToelbow),
                                    (int) shoulderToelbow.angle(elbowTowrist),
                                    (int) tricep_plane.angle(elbowTowrist),
                                    label};
                angles.add(angleSet);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return angles;
    }

    public String angleSetToString(ArrayList<Integer[]> a){
        StringBuilder bob = new StringBuilder();
        for(int i = 0; i < a.size(); i ++)
        {
            int j = 0;
            for(; j < a.get(i).length-1; j++)
                bob.append(a.get(i)[j]+" ");
            bob.append("| "+a.get(i)[j]+"\n");
        }
        return bob.toString();
    }

    public static void main(String args[]){
        AngleGen ag = new AngleGen("Gesture data");
        int[] x;
        while(ag.hasNext()){
            System.out.println(ag.angleSetToString(ag.next()));
        }
    }
}