function swc_standardize(output_file,rnames)

% Description: 
% 

    function [typeID_vector_modified] = replace_typeID(typeID_vector_modified, typeID_vector_original, to_replace, replace_with)
        index_list = find(typeID_vector_original==to_replace);
        typeID_vector_modified(index_list) = replace_with;
    end


    % SWC = readmatrix(output_file,'FileType','text','CommentStyle','#');

    fid = fopen(output_file,'r');
    SWC = textscan(fid, "%f %f %f %f %f %f %f", "CommentStyle", "#", "Delimiter", "\s");
    fclose(fid);
    SWC = cell2mat(SWC);

    [numRows,~] = size(SWC);
    typeID_vector_original = SWC(:,2);
    typeID_vector_modified = typeID_vector_original;

    for i = 1:length(rnames)
        
        regionName = rnames{i};
        regionName = lower(regionName);
        
        dendrite_match = ~isempty(strfind(regionName,'dend'));
        axon_match = ~isempty(strfind(regionName,'axon'));
        soma_match = ~isempty(strfind(regionName,'soma'));
        cellbody_match = ~isempty(strfind(regionName,'body'));
        glia_match = ~isempty(strfind(regionName,'glia'));
        noregions_are_defined = ~isempty(strfind(regionName,'noregions')); 
        
        % if noregions_are_defined
        %     typeID_vector_modified = replace_typeID(typeID_vector_modified, typeID_vector_original, i, 0);
        if soma_match || cellbody_match
            typeID_vector_modified = replace_typeID(typeID_vector_modified, typeID_vector_original, i, 1);
        elseif axon_match
            typeID_vector_modified = replace_typeID(typeID_vector_modified, typeID_vector_original, i, 2);
        elseif dendrite_match
            typeID_vector_modified = replace_typeID(typeID_vector_modified, typeID_vector_original, i, 3);
        elseif glia_match
            typeID_vector_modified = replace_typeID(typeID_vector_modified, typeID_vector_original, i, 7);
        else
            typeID_vector_modified = replace_typeID(typeID_vector_modified, typeID_vector_original, i, 0);
        end
             
    end
    
    SWC(:,2) = typeID_vector_modified;
    
    % make branchID same as parentID unless it's a root
    parent_vector = SWC(:,7);
    for i = 1:numRows
        rowID = typeID_vector_modified(i);
        if parent_vector(i)==-1
            continue
        else
            parentID = typeID_vector_modified(parent_vector(i));
            if rowID ~= parentID
                if parent_vector(i)~=1
                    typeID_vector_modified(i) = parentID;
                end
            end
        end
                
    end
    
    SWC(:,2) = typeID_vector_modified;

    dlmwrite (output_file, SWC, "delimiter", " ", "newline", "\n")

    % writematrix(SWC,output_file,'FileType','text','Delimiter','space')
    
end