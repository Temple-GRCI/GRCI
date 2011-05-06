package iitb.Segment;
import iitb.CRF.*;
/**
 *
 * @author Sunita Sarawagi
 *
 */ 

public interface TrainData extends DataIter {
    int size();   // number of training records
    @Override
    void startScan(); // start scanning the training data
    boolean hasMoreRecords(); 
    public TrainRecord nextRecord();
    @Override
    boolean hasNext(); 
    @Override
    public DataSequence next();
};

