package Kinect.Features;

import iitb.CRF.DataSequence;
import iitb.Model.*;
/**
 *
 * @author Christoph
 */
public class AngleFeatures extends FeatureTypes{
    private int numStates; //how many labels we use in train data
    private int stateId;  //which label [0-numStates)
    private int index;
    private Integer[] angleSet;

    public AngleFeatures(FeatureGenImpl fgen) {
	super(fgen);
        numStates=model.numStates();
    }
    @Override
    public boolean startScanFeaturesAt(DataSequence data, int prevPos, int pos) {
        //init stuff
        index = 0;
        stateId = 0;
        angleSet = (Integer[])data.x(pos);
        return true;
    }
    @Override
    public boolean hasNext() {
        return (index+1)*(stateId+1)<angleSet.length*numStates;
    }
    @Override
    public void next(FeatureImpl f) {
        setFeatureIdentifier(angleSet[index]+(360*stateId)+(360*numStates*index)+stateId,stateId,"A",f);
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