function mtr2swc(input_file, output_file)

[input_filepath, input_name, input_ext] = fileparts(input_file);

%% Check if output file is specified
if (nargin<2)
      output_file = strcat(input_filepath,'/',input_name,'.swc');
end

%% Launch Trees Toolbox
    % curentfolder=pwd;
    % cd './treestoolbox-master'; %verify if path is correct
    % echo off;
    % start_trees;
    % cd (curentfolder);

%% Main
    if (input_ext=='.mtr')
        tr = load_tree(input_file, 'none');
        if (iscell(tr))
            treestoolbox_tree = cell2mat(tr);
        else
            treestoolbox_tree = tr;
        end
        swc_tree(treestoolbox_tree, output_file);

    elseif (input_ext=='.mat')
        tr = load(input_file);
        if (iscell(tr))
            tr = cell2mat(tr);
        end
        if ~isstruct(tr)
            error("Unable to detect tree structure in .mat file.")
        end
        if ~anyisfield(tr, 'dA')
            error("Unable to detect tree structure in .mat file.")
        else
            fields = fieldnames(tr);
            found_tree = 0;
            while (numel(fields)~=0 && found_tree==0)
                for i = 1:numel(fields)
                    if isstruct(tr.(fields{i}))
                        if anyisfield(tr.(fields{i}), 'dA')
                            if isfield(tr.(fields{i}), 'dA')
                                found_tree = 1;
                                treestoolbox_tree = tr.(fields{i});
                                swc_tree(treestoolbox_tree, output_file);
                                break
                            else
                                fields = fieldnames(tr);
                                break
                            end
                        end
                    end
                end
            end
            if found_tree == 0
                error("Unable to detect tree structure in .mat file.")
            end
        end

    else
        error("Input file needs to be .mat or .mtr")
    end

% standardize typeID
if (exist('treestoolbox_tree') == 1)
    if isfield(treestoolbox_tree, 'rnames')
        swc_standardize(output_file, treestoolbox_tree.rnames)
    end
end

% exit;
