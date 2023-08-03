function [SWC_out] = swc_connect(SWC_in)

% Description: 
%   Called by bonfire_ndf2swc.  Links the obvious primary processes to the soma, and the obvious branch points into connected branches,
%   based on the thresholds 1 and 2 below.
% 
% Input:
%   SWC_in - input SWC file of unjoined processes
% 
% Output:
%   SWC_out - SWC file where obvious connections have been made
% 

% THRESHOLDS
thresh_1 = SWC_in(1,6)*1.5;
thresh_2 = 10;

origin_list = find(SWC_in(:,2) == 1);

for ii = 2:length(origin_list)
    x0 = SWC_in(1,3);
    y0 = SWC_in(1,4);
    O_O_dist = sqrt( (SWC_in(origin_list(ii),3)-x0)^2 + (SWC_in(origin_list(ii),4)-y0)^2 );
    
    if (O_O_dist <= thresh_1)
        SWC_in(origin_list(ii),2) = 3;
        SWC_in(origin_list(ii),7) = 1;
    else
    end
end

terminal_list = find(SWC_in(:,2) == 6);

for ii = 1:length(terminal_list)
    origin_list = find(SWC_in(:,2) == 1);
    origin_list = origin_list(2:end);
    
    T_O_dist = [];
    x0 = SWC_in(terminal_list(ii),3);
    y0 = SWC_in(terminal_list(ii),4);
    
    for qq = 1:length(origin_list)
        T_O_dist(qq) = sqrt( (SWC_in(origin_list(qq),3)-x0)^2 + (SWC_in(origin_list(qq),4)-y0)^2 );
    end
    
%     if ~isempty(find(T_O_dist <= thresh_2))
%         origin_indcies = find(T_O_dist <= thresh_2);
%         for kk = 1:length(origin_indcies)
%             SWC_in(origin_list(origin_indcies(kk)),7) = terminal_list(ii);
%         end
%     end
    
    if (length(find(T_O_dist <= thresh_2)) == 2)
        daughter_pointers = find(T_O_dist <= thresh_2);
        daughter_ind = [origin_list(daughter_pointers(1)) origin_list(daughter_pointers(2))]; 
        
        if (daughter_ind(1) > terminal_list(ii) && daughter_ind(2) > terminal_list(ii))
            SWC_in(daughter_ind(1),2) = 3;
            SWC_in(daughter_ind(1),7) = terminal_list(ii);
            SWC_in(daughter_ind(2),2) = 3;
            SWC_in(daughter_ind(2),7) = terminal_list(ii);
            SWC_in(terminal_list(ii),2) = 5;
        elseif (daughter_ind(1) > terminal_list(ii) && daughter_ind(2) < terminal_list(ii))
            SWC_in(daughter_ind(1),2) = 3;
            SWC_in(daughter_ind(1),7) = terminal_list(ii);
            SWC_in(terminal_list(ii),2) = 3;
        elseif (daughter_ind(1) < terminal_list(ii) && daughter_ind(2) > terminal_list(ii))
            SWC_in(daughter_ind(2),2) = 3;
            SWC_in(daughter_ind(2),7) = terminal_list(ii);
            SWC_in(terminal_list(ii),2) = 3;
        else
        end
    else
    end
    
        
end

SWC_out = SWC_in;