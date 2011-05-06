/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package PartOfSpeech;

import iitb.CRF.DataSequence;

/**
 *
 * @author Stephen
 */
public class PartSpeechSequence implements DataSequence {
    Object[] x;
    Integer[] y;
    public PartSpeechSequence(Object[] tokens,Integer[] labels){
        x=tokens;
        y=labels;
    }
    @Override
    public int length() {return x.length;}
    @Override
    public int y(int i) {return y[i];}
    @Override
    public Object x(int i) {return x[i];}
    @Override
    public void set_y(int i, int label) {y[i]=label;}
    @Override
    public String toString(){
        StringBuilder bob = new StringBuilder();
        for (int i=0; i<x.length; i++){
            bob.append(x[i]+" : "+y[i]+"\n");
        }

        return bob.toString();
    }
}
