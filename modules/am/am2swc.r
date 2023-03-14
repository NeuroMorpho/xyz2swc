args <- commandArgs(TRUE)

if (length(args)==0) {
  stop("At least one argument [input file] must be supplied!", call.=TRUE)
} else if (length(args)==1) {
  inputfile = args[1]
  file_ext = substr(inputfile,nchar(inputfile)-2,nchar(inputfile))
  if (file_ext == '.am'){
      outputfile = paste(substr(inputfile,1,nchar(inputfile)-3),'.swc',sep="")
  } else {
      stop("Input file needs to be of type .am", call.=TRUE)
  }
} else if (length(args)==2) {
  inputfile = args[1]
  outputfile = args[2]
  file_ext = substr(inputfile,nchar(inputfile)-2,nchar(inputfile))
  if ( file_ext != '.am'){
      stop("Input file needs to be of type .am", call.=TRUE)
  }
} else {
  stop("At most two arguments: [input file] and [output file]", call.=TRUE)
}

#suppressMessages(library(nat))
library(nat)

neuron1 = read.neuron(inputfile)
write.neuron(neuron1, outputfile, format='swc', Force=TRUE)
