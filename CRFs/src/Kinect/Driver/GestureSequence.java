/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Kinect.Driver;

import iitb.CRF.DataSequence;

/**
 *
 * @author Christoph
 */
public class GestureSequence implements DataSequence {
    private Integer[][] x;
    private Integer[] y;
    
    public GestureSequence(Integer[][] angleSets, Integer[] labels){
        x=angleSets;
        y=labels;
    }
    public GestureSequence(){
        x=new Integer[0][0];
        y=new Integer[0];
    }
    @Override
    public int length() {return y.length;}
    @Override
    public int y(int i) {return y[i];}
    @Override
    public Integer[] x(int i) {return x[i];}
    @Override
    public void set_y(int i, int label) {y[i]=label;}
    @Override
    public String toString(){
        StringBuilder bob = new StringBuilder();
        for(int i=0; i<length(); i++){
            for(Integer j: x(i))
                bob.append(j+" ");
            bob.append("| "+y(i)+"\n");
        }
        return bob.toString();
    }
}
