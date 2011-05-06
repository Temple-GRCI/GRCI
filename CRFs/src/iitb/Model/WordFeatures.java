package iitb.Model;
import iitb.CRF.*;
/**
 *
 * @author Sunita Sarawagi
 *
 */ 

public class WordFeatures extends FeatureTypes {
    int stateId;
    int statePos;
    Object token;
    int tokenId;
    WordsInTrain dict;
    int _numWordStatePairs;
    public static int RARE_THRESHOLD=0;
    public WordFeatures(FeatureGenImpl m, WordsInTrain d) {
	super(m);
	dict = d;
    }
    private void nextStateId() {       
	stateId = dict.nextStateWithWord(token, stateId);
	statePos++;
    }
    @Override
    public boolean startScanFeaturesAt(DataSequence data, int prevPos, int pos) {
	stateId = -1;
	if (dict.count(data.x(pos)) > RARE_THRESHOLD) {
	    token = (data.x(pos));
	    tokenId = dict.getIndex(token);
	    statePos = -1;
	    nextStateId();
	    return true;
	} 
	return false;
    }
    @Override
    public boolean hasNext() {
	return (stateId != -1);
    }
    @Override
    public void next(FeatureImpl f) {
        if (featureCollectMode())
            setFeatureIdentifier(tokenId*model.numStates()+stateId,stateId,"W_"+token,f);
        else
            setFeatureIdentifier(tokenId*model.numStates()+stateId,stateId,token,f); 
	f.yend = stateId;
	f.ystart = -1;
	f.val = 1;
	nextStateId();
    }
	/* (non-Javadoc)
	 * @see iitb.Model.FeatureTypes#maxFeatureId()
	 */
    @Override
	public int maxFeatureId() {
		return dict.dictionaryLength()*model.numStates();
	}
};


