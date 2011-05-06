/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Kinect.Driver;

import iitb.CRF.DataIter;
import iitb.CRF.DataSequence;

/**
 *
 * @author Christoph
 */
public class GestureIter implements DataIter {
    GestureSequence[] dataSeqs;
    int pos;
    
    public GestureIter(GestureSequence[] gseq){dataSeqs = gseq;}
    @Override
    public void startScan() {pos=0;}
    @Override
    public boolean hasNext() {return pos<dataSeqs.length;}
    @Override
    public DataSequence next() {return dataSeqs[pos++];}
}
