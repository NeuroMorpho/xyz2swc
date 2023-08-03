function [SWC_out] = swc_standardize(SWC_in, type_id_vector, no_soma_detected)

% Description: 
% 

[numRows,numCols] = size(SWC_in);

if (sum(SWC_in(1,3:6))==0) && no_soma_detected && (numel(type_id_vector) == (numRows-1))
    
    % remove fake soma row
    SWC_out = SWC_in(2:numRows,:); 
    
    % reset index number 
    SWC_out(:,1) = SWC_out(:,1)-1;
    
    %reset parent index which were roots
    SWC_out(:,7) = SWC_out(:,7)-1;
    SWC_out(SWC_out(:,7)==-2,7) = -1;
    
    %correct typeID
    SWC_out(:,2) = type_id_vector;
    
elseif numel(type_id_vector) == numRows
    SWC_out = SWC_in;
    SWC_out(:,2) = type_id_vector;
else
    SWC_out = SWC_in;
end


end