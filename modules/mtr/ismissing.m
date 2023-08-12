function TF = ismissing (A, indicator)

  if (nargin < 1) || (nargin > 2)
    print_usage ();
  endif

  ## check "indicator"
  if (nargin != 2)
     indicator = [];
  endif

  ## if A is an array of cell strings and indicator just a string,
  ## convert indicator to a cell string with one element
  if (iscellstr (A) && ischar (indicator) && ! iscellstr (indicator))
    indicator = {indicator};
  endif

  if ((! isempty (indicator)) &&
      ((isnumeric (A) && ! (isnumeric (indicator) || islogical (indicator))) ||
       (ischar (A) && ! ischar (indicator)) ||
       (iscellstr (A) && ! (iscellstr (indicator)))))
    error ("ismissing: 'indicator' and 'A' must have the same data type");
  endif

  ## main logic
  if (isempty (indicator))

    if (isnumeric (A))
      ## numeric matrix: just find the NaNs
      ## integer types have no missing value, but isnan will return false
      TF = isnan (A);

    elseif (iscellstr (A))
      ## cell strings - find empty cells
      TF = cellfun ('isempty', A);

    elseif (ischar (A))
      ## char matrix: find the white spaces
      TF = isspace (A);

    else
      ##no missing type defined, return false
      TF = false (size (A));
    endif

  else
    ## indicator specified for missing data
    TF = false (size (A));
    if (isnumeric(A) || ischar (A) || islogical (A))
      for iter = 1 : numel (indicator)
        if (isnan (indicator(iter)))
          TF(isnan(A)) = true;
        else
          TF(A == indicator(iter)) = true;
        endif
      endfor
    elseif (iscellstr (A))
      for iter = 1 : numel (indicator)
        TF(strcmp (A, indicator(iter))) = true;
      endfor
    else
      error ("ismissing: indicators not supported for data type '%s'", ...
               class(A));
    endif
  endif
endfunction