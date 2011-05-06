/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Kinect.Driver;

import Kinect.Features.*;
import iitb.Model.*;

/**
 *
 * @author Christoph
 */
public class GestureFeatureGenImpl extends FeatureGenImpl {
    public GestureFeatureGenImpl(String modelSpecs, int numLabels) throws Exception {
        super(modelSpecs,numLabels,true);
    }
    @Override
    protected void addFeatures(){
        addFeature(new AngleFeatures(this));
        addFeature(new EdgeFeatures(this));
        addFeature(new PrevAngleFeatures(this));
    }
}
