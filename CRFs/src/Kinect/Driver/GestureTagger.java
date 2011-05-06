package Kinect.Driver;

import Kinect.AngleGenerator.AngleGen;
import java.io.*;
import java.util.*;
import iitb.CRF.*;
import iitb.Model.*;
import iitb.Utils.*;

/**
 *
 * @author Christoph
 */
public class GestureTagger {

    String inName="gesture";
    String outDir="gesture";
    String baseDir="my_samples";
    String modelGraphType = "naive";
    int nlabels=37;
    int windowSize=75;

    Options options = new Options();
    CRF crfModel;
    GestureFeatureGenImpl featureGen;

    public static void main(String argv[]) throws Exception {
	    /*
	     * Initialization:
	     * Get the required arguements for the application here.
	     * Also, you will need to create a Properties object for arguements to be
	     * passed to the CRF. You do not need to worry about this object,
	     * because there are default values for all the parameters in the CRF package.
	     * You may need to pass your own parameters values for tuning the application
	     * performance.
	     */
        /*if (argv.length < 3) {
	    System.out.println("Usage: java Tagger train|test|calc -f <conf-file>");
	    return;
	}*/
        GestureTagger tagger= new GestureTagger();
	    /*
	     * There are mainly two phases for a learning application: Training and Testing.
	     * Implement two routines for each of the phases and call them appropriately here.
	     */
	if (argv[0].toLowerCase().indexOf("train")>=0) {
            tagger.train();
        }
	if (argv[0].toLowerCase().indexOf("test")>=0) {
            tagger.test();
        }
	if (argv[0].toLowerCase().indexOf("calc")>=0) {
	    tagger.calc();
        }
    }

    public void train() throws Exception {
        /*
         * Read the training dataset into an object which implements DataIter
         * interface(trainData). Each of the training instance is encapsulated in the
         * object which provides DataSequence interface. The DataIter interface
         * returns object of DataSequence (training instance) in next() routine.
         */
        GestureIter trainData = new GestureIter(encapsulateLabeledData(baseDir+"/rawSamples/"+inName+"/TrainData"));
        /*
         * Once you have loaded the training dataset, you need to allocate objects
         * for the model to be learned. allocmodel() method does that allocation.
         */
        allocModel();

        /*
         * You may need to train some of the feature types class. This training is
         * needed for features which need to learn from the training data for instance
         * dictionary features build generated from the training set.
         */
        featureGen.train(trainData);

        /*
         * Call train routine of the CRF model to train the model using the
         * train data. This routine returns the learned weight for the features.
         */
        double featureWts[] = crfModel.train(trainData);

        /*
         * You can store the learned model for later use into disk.
         * For this you will have to store features as well as their
         * corresponding weights.
         */
        crfModel.write(baseDir+"/learntModels/"+outDir+"/crf");
        featureGen.write(baseDir+"/learntModels/"+outDir+"/features");

    }

    public void test() throws Exception {
        PrintWriter fout = new PrintWriter(new File(baseDir+"/data/"+inName+"/test.txt"));
        /*
         * Read the test dataset. Each of the test instance is encapsulated in the
         * object which provides DataSequence interface.
         */

        /*
         * Once you have loaded the test dataset, you need to allocate objects
         * for the model to be learned. allocmodel() method does that allocation.
         * Also, you need to read learned parameters from the disk stored after
         * training. If the model is already available in the memory, then you do
         * not need to reallocate the model i.e. you can skip the next step in that
         * case.
         */
        allocModel();
        featureGen.read(baseDir+"/learntModels/"+outDir+"/features");
        crfModel.read(baseDir+"/learntModels/"+outDir+"/crf");

        /*
         * Iterate over test data set and apply the crf model to each test instance.
         */
        GestureSequence testRecord = new GestureSequence();
        GestureIter gestIt = new GestureIter(encapsulateLabeledData(baseDir+"/rawSamples/"+inName+"/LabeledTestData"));
        gestIt.startScan();
        while(gestIt.hasNext()) {
            testRecord = (GestureSequence)gestIt.next();
            /*
             * Now apply CRF model to each test instance.
             */
            crfModel.apply( testRecord );
            /*
             * The labeled instance have value of the states as labels.
             * These state values are not labels as supplied during training.
             * To map this state to one of the labels you need to call following
             * method on the labled testRecord.
             */
            featureGen.mapStatesToLabels( testRecord );
            fout.print(testRecord.toString());
        }
        fout.close();
    }

    void allocModel() throws Exception {
        /*
         * A CRF model consists of features and corresponding weights.
         * The features are stored in FeatureGenImpl and weights and other
         * CRF parameters are encapsulated in CRF object.
         *
         * Here, you will call appropriate constructor for a feature generator
         * and a CRF model. You can use feature generator available in the
         * package or use your own implemented feature generator.
         *
         * There are two CRF model classes: CRF and NestedCRF. The CRF class is
         * flat CRF model while NestedCRF is a segment(semi-)CRF model.
         */
        featureGen = new GestureFeatureGenImpl("naive",nlabels);
        crfModel = new CRF(featureGen.numStates(),featureGen,options);
    }

    public GestureSequence[] encapsulateLabeledData(String path) throws FileNotFoundException{
        PrintWriter fout = new PrintWriter(new File(baseDir+"/data/"+inName+"/control.txt"));
        AngleGen angleGen = new AngleGen(path);
        ArrayList<GestureSequence> seqSet = new ArrayList<GestureSequence>();

        ArrayList<Integer[]> angleSets;
        int numWindows;

        ArrayList<Integer> yPrime = new ArrayList<Integer>();
        ArrayList<Integer[]> xPrime = new ArrayList<Integer[]>();

        int label=-1;

        while (angleGen.hasNext()){
            angleSets = angleGen.next();
            numWindows = angleSets.size()-windowSize+1;
            for (int h=0;h<numWindows;h++){
                for (int i=0;i<50;i++){
                    Integer[] angleSet = new Integer[4];
                    Integer[] set = angleSets.get(i+h);//set has 5 elements: 4 angles and their label.
                    label = set[4];
                    for (int j=0;j<set.length-1;j++)
                        angleSet[j]=set[j];
                    xPrime.add(angleSet);
                    yPrime.add(label);
                }
                
                if (xPrime.size()>0){
                    Integer[][] x = xPrime.toArray(new Integer[xPrime.size()][4]);
                    Integer[] y = yPrime.toArray(new Integer[yPrime.size()]);
                    xPrime.clear();
                    yPrime.clear();
                    GestureSequence gs = new GestureSequence(x,y);
                    seqSet.add(gs);
                    fout.print(gs.toString());
                }
            }
        }
        fout.close();
        return seqSet.toArray(new GestureSequence[seqSet.size()]);
    }

    public void calc() throws FileNotFoundException{
        Scanner cin = new Scanner(new File(baseDir+"/data/"+inName+"/control.txt"));
        Scanner tin = new Scanner(new File(baseDir+"/data/"+inName+"/test.txt"));
        String cline;
        String tline;
        Integer clabel, tlabel, count;
        int score=0;

        Map<Integer,Integer> labelCount = new HashMap<Integer,Integer>();
        for (int i=0;i<featureGen.numStates();i++)
            labelCount.put(i, 0);

        while(cin.hasNextLine()){
            jumpLines(cin,windowSize);
            jumpLines(tin,windowSize);
            if (cin.hasNextLine() && tin.hasNextLine()){
                cline = cin.nextLine().trim();
                tline = tin.nextLine().trim();
                clabel = Integer.parseInt(""+cline.charAt(cline.length()-1));
                tlabel = Integer.parseInt(""+tline.charAt(tline.length()-1));

                count = labelCount.get(clabel);
                labelCount.put(clabel,count+1);
                if (clabel==tlabel)
                    score++;
            }
        }

        int sampleSize = 0;
        int mostFreq = 0;
        int tmp;
        for (int i=0;i<labelCount.size();i++){
            tmp = labelCount.get(i);
            if (labelCount.get(mostFreq)<tmp)
                mostFreq = i;
            sampleSize += tmp;
        }
        double baseLine = labelCount.get(mostFreq)/(double)sampleSize*100;
        double accuracy = score/(double)sampleSize*100;
        System.out.println("Score: "+score+":"+sampleSize);
        System.out.println("Most Frequent Label: "+mostFreq+":"+labelCount.get(mostFreq));
        System.out.println("Accuracy: "+accuracy+"%\n"+"Baseline: "+baseLine+"%\n");
    }

    public boolean jumpLines(Scanner scanner, int offset){
        int i=0;
        for (;scanner.hasNextLine() && i<offset-1;i++)
            scanner.nextLine();
        if (i==offset-1) return true;
        else return false;
    }
}