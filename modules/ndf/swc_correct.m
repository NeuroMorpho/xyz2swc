function [SWC_out] = swc_correct(SWC_in)

% Description: Called by bonfire_ndf2swc.  Identifies branch points and terminal points and relables them as such.
% 
% Input:
%   SWC_in - a matrix describing neuronal morphology in .swc format
% 
% Output:
%   SWC_out - a matrix describing neuronal morphology in .swc format
% 

SWC_out = SWC_in;

% Branching point and terminal point check.  For each node, we check the number of daughter nodes, and relabel the point accordingly
for ii = 1:size(SWC_in,1)
    n = SWC_in(ii,1);
    daughter_counter = 0;
    
%     Find daughter nodes (nodes where the parent node (P) is equal to the node we are trying to find daughters for (n)
    for jj = 1:size(SWC_in,1)
        P = SWC_in(jj,7);
        if (P == n)
            daughter_counter = daughter_counter + 1;
        else
        end
    end
    
%     Label branch points (nodes with 2+ daughters)
    if (daughter_counter >= 2 && SWC_in(ii,7) ~= -1)
        SWC_out(ii,2) = 5;
        
%     Label terminal points (nodes with no daughters)
    elseif (daughter_counter == 0 && SWC_in(ii,7) ~= -1)
        SWC_out(ii,2) = 6;
    end
end