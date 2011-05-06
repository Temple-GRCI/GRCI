package Kinect.Features;

import iitb.CRF.DataSequence;
import iitb.Model.*;
/**
 *
 * @author Christoph
 */
public class PrevAngleFeatures extends FeatureTypes{
    private int stateId;
    private int numStates;
    private int index;
    private Integer[] x_i;
    private Integer[] x_iMinusOne;
    private int[] delta_x;

    public PrevAngleFeatures(FeatureGenImpl fgen){
        super(fgen);
        numStates=model.numStates();
        delta_x = new int[4];

    }
    @Override
    public boolean startScanFeaturesAt(DataSequence data, int prevPos, int pos) {
        if (pos>=1){
            x_i = (Integer[])data.x(pos);
            x_iMinusOne = (Integer[])data.x(prevPos);
            for(int k=0;k<x_i.length;k++)
                delta_x[k] = x_i[k] - x_iMinusOne[k];
            stateId = 0;
            index = 0;
            return true;
        }else {
            x_i = new Integer[0];
            return false;
        }
    }
    @Override
    public boolean hasNext() {
        return (index+1)*(stateId+1)<numStates*x_i.length;
    }
    @Override
    public void next(FeatureImpl f) {
        setFeatureIdentifier(delta_x[index]+(720*numStates*index)+(720*stateId),stateId,"PA",f);
        f.yend = stateId;
	f.ystart = -1;
	f.val = 1;
        nextStateId();
    }

    public void nextStateId(){
        if (stateId == numStates-1){
            stateId = 0;
            index++;
        }else
            stateId++;
    }
}
