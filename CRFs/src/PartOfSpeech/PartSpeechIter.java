/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package PartOfSpeech;

import iitb.CRF.DataIter;
import iitb.CRF.DataSequence;

/**
 *
 * @author Stephen
 */
public class PartSpeechIter implements DataIter{
    PartSpeechSequence[] data;
    int pos;
    
    public PartSpeechIter(PartSpeechSequence[] pseq){data = pseq;}
    @Override
    public void startScan() {pos=0;}
    @Override
    public boolean hasNext() {return pos<data.length;}
    @Override
    public DataSequence next() {return data[pos++];}

}
