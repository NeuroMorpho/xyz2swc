function ndf2swc(input_file, output_file, scale_factor)

[input_filepath, input_name, input_ext] = fileparts(input_file);

% Check if output file is specified
if (nargin<2)
    if(isempty(input_filepath))
        output_file = strcat(input_name,'.swc');
    else
        output_file = strcat(input_filepath,'/',input_name,'.swc');
    end
end

% Check if scale factor is specified
if (nargin<3)
    scale_factor=10;
end

if (~strcmp(input_ext,'.ndf'))
    disp("Error! File is not of type .ndf")
    exit;
end

contains = @(str, pattern) ~cellfun('isempty', strfind(str, pattern));

F = fileread(input_file);
F =  strsplit(F,"\n");
% F = readlines(input_file);
default_indicies = find(contains(F,"Default",'IgnoreCase',true));
default_indicies = default_indicies + 1;
tracing_indicies = find(contains(F,"// Tracing",'IgnoreCase',true));
type_indicies = tracing_indicies + 2;

if numel(tracing_indicies)==0 || numel(default_indicies)==0
    is_standard_NDF = false;
    tracing_indicies = find(contains(F,"Tracing",'IgnoreCase',true));
else
    is_standard_NDF = true;
end


Px = {};
Py = {};
k = 0;

if is_standard_NDF

    for i = 1:numel(type_indicies)
        type_id(i) = str2num(F{type_indicies(i)});
    end

    for d = 1:numel(tracing_indicies)
        i = tracing_indicies(d)+5;
        if (startsWith(F{i},'// Segment','IgnoreCase',true))
            j = i+1;
            k = k + 1;
            l = 1;
            dont_increment_k = true;
            while(dont_increment_k == true)
                if(startsWith(F{j},'// Segment','IgnoreCase',true))
                    j = j+1;
                elseif(startsWith(F{j},'// Tracing','IgnoreCase',true))
                    dont_increment_k = false;
                    continue
                elseif(startsWith(F{j},'// End','IgnoreCase',true))
                    dont_increment_k = false;
                    break
                else
                    Px{k}(l) = str2num(F{j});
                    Py{k}(l) = str2num(F{j+1});
                    j = j+2;
                    l = l+1;
                end
            end
        end
    end %d

else
    type_id = zeros(1,numel(tracing_indicies));

    for k = 1:numel(tracing_indicies)
        increment_i = true;
        i = tracing_indicies(k);
        l = 1;
        while increment_i
            i = i+1;
            if i>numel(F)
                break
            end
            if (startsWith(F{i},'Tracing','IgnoreCase',true))
                increment_i = false;
                continue
            end
            line_i_split = split(F{i});
            if numel(line_i_split)==2
                Px{k}(l) = str2num(line_i_split{1});
                Py{k}(l) = str2num(line_i_split{2});
                l = l+1;
            end
        end
    end %k
end


is_cellbody = find(type_id==6);
is_process = find(type_id~=6);

if isempty(is_cellbody)
    no_soma_detected = true;
    type_id_vector = [];
else
    no_soma_detected = false;
    type_id_vector = [1];
end
    
for i = 1:numel(type_id)
    if type_id(i) == 6
        continue
    else
        temp_len = length(Px{i});
        if type_id(i) == 1
            temp_id = 2;
        elseif type_id(i) == 2
            temp_id = 3;
        else
            temp_id = 0;
        end
        type_id_vector = [type_id_vector, temp_id.*ones(1,temp_len)];
    end
        
end


if no_soma_detected
    cell_body(:,:,1) = 0;
    cell_body(:,:,2) = 0;
else
    cell_bodyX = Px(:,is_cellbody);
    cell_bodyY = Py(:,is_cellbody);
    max_length = max(cellfun(@numel,cell_bodyX));
    
    cell_bodyX = cellfun(@(x) [x,zeros(1,max_length-numel(x))],cell_bodyX,'UniformOutput',false);
    cell_bodyY = cellfun(@(x) [x,zeros(1,max_length-numel(x))],cell_bodyY,'UniformOutput',false);

    cell_bodyX = reshape(cell2mat(cell_bodyX),max_length,numel(cell_bodyX));
    cell_bodyY = reshape(cell2mat(cell_bodyY),max_length,numel(cell_bodyY));
    
    cell_body(:,:,1) = cell_bodyX;
    cell_body(:,:,2) = cell_bodyY;

end
    
% cell_bodyX = Px{1}';
% cell_bodyY = Py{1}';
% 
% cell_body = [];
% cell_body(:,:,1) = cell_bodyX;
% cell_body(:,:,2) = cell_bodyY;

ProcessesX = Px(:,is_process);
ProcessesY = Py(:,is_process);
max_length = max(cellfun(@numel,ProcessesX));

ProcessesX = cellfun(@(x) [x,zeros(1,max_length-numel(x))],ProcessesX,'UniformOutput',false);
ProcessesY = cellfun(@(x) [x,zeros(1,max_length-numel(x))],ProcessesY,'UniformOutput',false);

ProcessesX = reshape(cell2mat(ProcessesX),max_length,numel(ProcessesX));
ProcessesY = reshape(cell2mat(ProcessesY),max_length,numel(ProcessesY));



[Lp, Np] = size(ProcessesX);
ndf2processes = zeros(Lp,Np,2);
processes(:,:,1) = ProcessesX;
processes(:,:,2) = ProcessesY;

SWC = ndf_main(processes,cell_body,scale_factor);
SWC = swc_correct(SWC);
SWC = swc_connect(SWC);
SWC = swc_correct(SWC);

% standadrdize
% type_id_vector(indicies_toremove) = [];
SWC = swc_standardize(SWC, type_id_vector, no_soma_detected);

% writematrix(SWC,output_file,'FileType','text','Delimiter','space')
dlmwrite (output_file, SWC, "delimiter", " ", "newline", "\n")

% exit

end
