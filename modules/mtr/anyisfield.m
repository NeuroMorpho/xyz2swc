function out_isfieldresult = anyisfield(in_rootstruct,in_field)
%ANYISFIELD: Extension of built in isfield function. Added features:
%1. Multilevel inputs (e.g. "b.c.d")
%2. Identification of matches at any level of nested structs
contains = @(str, pattern) ~cellfun('isempty', strfind(str, pattern));

in_field = num2str(in_field);
rootfieldnames = fieldnames(in_rootstruct(1));
out_isfieldresult = false;
if contains(cellstr(in_field),"[.]")
    current_searchfieldname = extractBefore(in_field,"[.]");
else
    current_searchfieldname = in_field;
end
remaining_searchfieldnames = extractAfter(in_field,"[.]");
for i=1:length(rootfieldnames)
    if strcmp(rootfieldnames{i},current_searchfieldname)
        if ismissing(remaining_searchfieldnames)
            out_isfieldresult = true;
            return
        else
            out_isfieldresult = anyisfield(in_rootstruct(1).(current_searchfieldname),remaining_searchfieldnames);
        end
    elseif isstruct(in_rootstruct(1).(rootfieldnames{i}))
        out_isfieldresult = anyisfield(in_rootstruct(1).(rootfieldnames{i}),current_searchfieldname);
        if out_isfieldresult == true
            return;
        end
    end
end
end
