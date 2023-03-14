# SWC Standardization

In general, we assume that we do not know the reconstruction software (or source) of the `.swc` file. To determine if the file meets the SWC standard specification, we perform the list of checks outlined below on every input `.swc`.

The standardization operation consists of two phases:
## Check Phase
In this phase we perform a series of checks (and sub-checks) to detect formatting errors. This is a "read only" operation, and no edits are made to the .swc file.

Implemented in the code using the `xyz2swc.standard.chksingle` function.

```
Method for implementing the 'check phase' during standardization.

Required arguments:
        filename:          [string]     Path of the SWC file to check.

Returns:
        check_list_df:     [pandas.df]  Log of the formatting errors.
        conversion_flags:  [tuple]      Collection of flags that are used by swccorrect(), determining what correction actions need to be performed.
```

<br/>

## Correction Phase
The correction phase is run on a case-by-case basis depending on the output of the check phase. Based on the detected non-standard errors in the input file, a standardized version of the `.swc` file is generated. Further, the set of 'actions' performed to standardize the `.swc` are detailed in the generated log. Note that not all non-standard errors can be corrected.

Implemented in the code using the `xyz2swc.standard.swccorrect` function.

```
Method for implementing the 'correction phase' during standardization.
Cannot be run independetly without first running chksingle().

Required arguments:
        filename:          [string]     Path of the SWC file to correct.
        check_list_df:     [pandas.df]  Log of formatting errors detected by chksingle().
        conversion_flags:  [tuple]      Collection of flags that are set by chksingle(), determining what correction actions need to be performed.

Returns:
        correction_list_df: [pandas.df] Log of formatting errors + corrective actions performed.
```

<br/>

## UI Implementation
The UI has two action buttons "Convert/Standardize" and "Check".

If the "Check" action is requested by the user then only the check phase is implemented, and the output is a log file (list of errors).

If the "Convert/Standardize" action is requested by the user then both phases are implemented sequentially, and the output is a standardized `.swc` file along with a log file (list of errors and corresponding corrective actions performed).

<br/>

## List of Checks

Legend:
    :heavy_check_mark: Changes have been pushed on to GitLab from my end.
    :x: Not implemented even locally.
<!-- :heavy_plus_sign: Not implemented on web UI yet; -->

#### 1. Missing Field Columns :heavy_check_mark:
- *Check:* Does the swc matrix have seven columns? If not then return an error. All further checks omitted
- *Sub-Check:* -
- *Correction:* -

#### 2. No. of lines :heavy_check_mark:
- *Check:* Detect number of tree samples.
- *Sub-Check:* File too short: If the `.swc` file has less than 20 samples, generate a warning.
- *Correction:* -

#### 3. No. of soma samples :heavy_check_mark:
- *Check:* Detect number of soma samples. Generate warning if no soma samples detected.
- *Sub-Check:* If no samples with `TypeID=1` are detected, check if it's a SWCPlus file.
- *Correction:* If SWCPlus file detected, then set soma TypeID=1.

#### 4. Root ParentIndex :heavy_check_mark:
- *Check:* Does the first sample in the file have `ParentIndex =-1`?
- *Sub-Check:* What if more than one -1?
* *Corrections:*
    * Make sample with `ParentIndex =-1` as the first sample in the file.
    * If no sample with `ParentIndex =-1`, then generate error.

#### 5. Invalid ParentIndex :heavy_check_mark:
- *Check:* Does the `ParentIndex` point to an `Index` value that does not exist? If not generate an error.
- *Sub-Check:* -
- Correction:
    - Generate an error.
    - Set invalid `ParentIndex=-1`, i.e., make the sample a root point.

#### 6. NonStandard TypeID :heavy_check_mark:
- *Check:* Is `TypeID` an integer value in the range 1-7.
- *Sub-Checks:*
    - (NeuronStudio check) If bifurcations have `TypeID=5` and terminals have `TypeID=6`.
    - If `TypeID=0` then generate an error: sample undefined.
- *Corrections:*
    - (NeuronStudio error) Set bifurcation `TypeID` to that of branch, and terminal `TypeID` to that of parent.
    - All `TypeID=0` are converted into `TypeID=6` indicating 'unspecified neurite'.

#### 7. ITP int :heavy_check_mark:
- *Check:* Are the `Index`, `TypeID`, and `ParentIndex` fields integer values?
- *Sub-Check:* `Index` or `ParentIndex` is not zero
- *Correction:* If `Index=0` reset numbering starting from 1.

#### 8. XYZ double :heavy_check_mark:
- *Check:* Are the `X`, `Y`, and `Z` coordinates float/double values?
- *Sub-Checks:* Decimal comma used in place of decimal point?
- *Correction:* Replace decimal comma with decimal point

#### 9. Radius positive double :heavy_check_mark:
- *Check:* Is sample `Radius` a double/float value?
- *Sub-Check:*
    - Is `Radius<=0`?
    - All `Radius` are set with the same "default" value?
    - Decimal comma used in place of decimal point?
- *Correction:*
    - If radius is negative or radius is zero set default `Radius=0.5`.
    - Replace decimal comma with decimal point

#### 10. Index sequential :heavy_check_mark:
- *Check:* Are the `Index` values in sequential order starting from 1?
- *Sub-Check:* Requires Check#7 PASS.
- *Correction:* Reset `Index` numbering.

#### 11. Sorted Index Order :heavy_check_mark:
- *Check:* Are parent samples defined before they are being referred to?
- *Sub-Check:* Requires Check#5 PASS.
- *Correction:* Sort and reset `Index` and `ParentIndex` numbering.

#### 12. Contour :heavy_check_mark:
- *Check:* Is the soma segment a contour?
- *Sub-Check:*
    - Are there 3 or more soma samples? If not abort contour check.
    - Detect curvature angle of the soma segment.
    - Convert all `.dat` files first to `.asc` first. Check for contour. Then convert to `.swc`.
- *Correction:* If the curvature angle is acute, declare segment to be a contour, and replace it with a single point soma.

#### 13. Abnormal Compartments :heavy_check_mark:
- *Check:* (Spider leg check) Are there unusually long compartments?
- *Sub-Check:* Long compartments must be above the threshold of 10um.
- *Correction:*
    - Default: Warning
    - First option: Only disconnect if the parent is a root.
    - Second option: Cut long connections even if parent is not a root.
    - Thrid option: Can be reattached at half the distance.
- *User Input:* Specify which option to perform.


<br/><br/>

---

## Detecting Abnormal Compartment Lengths

1. Calculate length of each compartment in the tree.
2. Calculate the standard deviation $sd$ across all compartment-lengths.
3. If the length of a particular compartment is greater than $n\!\times\!sd$, then we determine the compartment length to abnormally high. The current choice for $n=5$.
<!--> 4. A minimum threshold of 10um to take into account any possible manual edits that might have been made to the reconstructions. <--->



<br/><br/>

---

## Soma Contour Check

### Step 1:
Start with the `.swc`, regardless of the original format the `.swc` was generated from.

### Step 2: Extract soma samples

Soma samples in the generated `.swc` files have a `TypeID = 1`. This is currently true for all `.swc` files generated by NeuronLand and most of the modules used in *xyz2swc*. We extract and process only these samples.

What if the contour is not marked by `1`? Then check the curvature of all segments with parent node as the root `ParentIndex = -1`.

Note: A segment is defined as a set of contiguous samples with a start-point and end-point. The start-point of a segment can in general be either the root or a bifurcation. The end-point is either a bifurcation or terminal. A soma segment always has its start-point as the root.

### Step 3: Contour or Cylinder

To determine whether a soma segment is a contour or a cylinder we calculate the curvature angle $\theta$ as shown.
![](https://i.imgur.com/46lcx0h.png)

Let $X, Y, Z$ denote the start-point, mid-point, and end-point of the soma segment, respectively. The mid-point is defined as the sample along the segment that is furthest away from both the start and end points. Then $\theta := \angle XYZ$ is defined to be the curvature angle of the segment.
* If curvature angle is acute, $\theta <= 90^\circ$, we determine the soma segment to be a contour.
* If curvature angle is obtuse, $\theta > 90^\circ$, we determine the soma segment to be a cylinder.

If the segment is a contour we replace it by a single sample located at the center corresponding to the average of all sample points of that contour. An equivalent radius is computed as the average distance of each sample point in the contour from this center.

If the segment is a cylinder we retain all samples 'as is'.


### Multiple contours as handled by NeuronLand

The code and steps above are robust enough to work for multiple contours (determine the corresponding center and equivalent radius of each contour), and cylinders. However, note that these steps are applied only to the `.swc` file and not the original format file.

`.swc` files generated by NeuronLand reduce multiple contours into a single contour during the conversion process. An example of this is [28-SN-4 - Balleine](http://neuromorpho.org/neuron_info.jsp?neuron_name=28-SN-4), where the `.swc` generated by Neuronland does not retain the information of the multiple contours.
![](https://i.imgur.com/y2rQoil.png)

We will need an optional (radio button) for users to specify if their `.dat` file has multiple contours. If yes, use NeuronLand to convert `.dat`$\rightarrow$`.asc`, and read $x,y,z$ co-ordinates of each contour from the `.asc`. Then proceed to convert each contour into a single point soma.
