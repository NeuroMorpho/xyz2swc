
##
## Configure build
$NinjaExecutable	= "../3rdParty/Ninja/ninja.exe"
$TestExecutable 	= "../solution_vs2017/bin/x64/Release/NeuronMorphologyFormatConverter.exe"
$NinjaBuildFile		= "build.ninja"
$NeuroMorphoArchive = "data/TestData_NeuroMorpho_6_3"


##
## Make ninja build file

# rule
New-Item $NinjaBuildFile -Force
Add-Content $NinjaBuildFile "format = Unspecified" # Each build command needs to specify a valid format
Add-Content $NinjaBuildFile "rule nlmc"
Add-Content $NinjaBuildFile "  command = $($TestExecutable) --test `$in `$format `$out"
Add-Content $NinjaBuildFile ""

# prepare the files in a specific directory
function Ninja-Add-Test-Archive() {
	Param (
        [Parameter(Mandatory=$true)][string[]]$NinjaBuildFile,
        [Parameter(Mandatory=$true)][string[]]$InputDir,
        [Parameter(Mandatory=$true)][string[]]$OutputFilePrefix,
        [Parameter(Mandatory=$true)][string[]]$MorphologyFormat,
        [Parameter(Mandatory=$false)][string[]]$FileextFilter
    )
	
	if ($FileextFilter -eq '') { $FileextFilter = '*' }
	
	Get-ChildItem $InputDir -File -Filter *.$FileextFilter |
	Foreach-Object {

		# ninja doesn't like raw spaces in file names, so need to escape spaces with $ 
		$InputDir_nj = ([String]$InputDir).Replace(" ","$ ")
		$OutputFilePrefix_nj = ([String]$OutputFilePrefix).Replace(" ","$ ")
		#

		$filename = "build $($OutputFilePrefix_nj)/$($_.BaseName).$($_.Extension).test: nlmc $($InputDir_nj)/$($_.Name)"
	
		Add-Content $NinjaBuildFile $filename
		Add-Content $NinjaBuildFile "  format = $($MorphologyFormat)"
	}
}

# Helper, specific to Neuromorpho archive layout
function Ninja-Add-NeuroMorpho-Test-Archive {
	Param (
        [Parameter(Mandatory=$true)][string[]]$NinjaBuildFile,
        [Parameter(Mandatory=$true)][string[]]$ArchiveRoot,
        [Parameter(Mandatory=$true)][string[]]$ArchiveName,
        [Parameter(Mandatory=$true)][string[]]$MorphologyFormat,
        [Parameter(Mandatory=$false)][string[]]$FileextFilter
    )

	$SourceDirSRC = "$($ArchiveRoot)/$($ArchiveName)/Source-Version"
	$SourceDirCNG = "$($ArchiveRoot)/$($ArchiveName)/CNG Version"

	$OutputFilePrefixSRC = "TestResults/$($ArchiveName)/Source-Version"
	$OutputFilePrefixCNG = "TestResults/$($ArchiveName)/CNG Version"

	Ninja-Add-Test-Archive $NinjaBuildFile $SourceDirSRC $OutputFilePrefixSRC $MorphologyFormat $FileextFilter
	Ninja-Add-Test-Archive $NinjaBuildFile $SourceDirCNG $OutputFilePrefixCNG "SWC"
	
	Write-Output "Added archive `"$($ArchiveName)`""
}

$archive_list = @(
	[pscustomobject]@{name='acsady';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='allman';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='amaral';format='Eutectics';extFilter='*'}
	[pscustomobject]@{name='araujo';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='ascoli';format='SWC';extFilter='*'}
	[pscustomobject]@{name='ascoli+buzsaki';format='SWC';extFilter='*'}
	[pscustomobject]@{name='barbour';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='baro';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='barrionuevo';format='SWC';extFilter='swc'}
	[pscustomobject]@{name='barrionuevo';format='Eutectics';extFilter='txt'}
	[pscustomobject]@{name='barrionuevo+diadem';format='SWC';extFilter='*'}
	[pscustomobject]@{name='bausch';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='bergstrom';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='bikson';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='borst';format='SWC';extFilter='*'}
	[pscustomobject]@{name='brown';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='brumberg';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='brunjes';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='burke';format='SWC';extFilter='*'}
	[pscustomobject]@{name='buzsaki';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='cameron';format='Eutectics';extFilter='*'}
	[pscustomobject]@{name='cauli';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='chalupa';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='chmykhova';format='Eutectics';extFilter='*'}
	[pscustomobject]@{name='claiborne';format='Nevin';extFilter='*'}
	[pscustomobject]@{name='cox';format='SWC';extFilter='*'}
	[pscustomobject]@{name='danzer';format='NeurolucidaASC';extFilter='*ASC'}
	[pscustomobject]@{name='danzer';format='NeurolucidaDAT';extFilter='DAT'}
	[pscustomobject]@{name='defelipe';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='dendritica';format='SWC';extFilter='swc'}
	[pscustomobject]@{name='dendritica';format='NeuronHOC';extFilter='nrn'}
	[pscustomobject]@{name='dendritica';format='NeuronHOC';extFilter='hoc'}
	[pscustomobject]@{name='destexhe';format='NeuronHOC';extFilter='*'}
	[pscustomobject]@{name='diadem+ascoli+sugihara';format='SWC';extFilter='*'}
	[pscustomobject]@{name='eysel';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='fyffe';format='Eutectics';extFilter='*'}
	[pscustomobject]@{name='garcia-cairasco';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='glickfeld_scanziani';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='gonzalez-burgos';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='guerra da rocha';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='gulyas';format='SWC';extFilter='*'}
	[pscustomobject]@{name='helmstaedter';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='hirsch';format='SWC';extFilter='*'}
	[pscustomobject]@{name='hirsch+diadem';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='jacobs';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='jacobs_g';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='jaeger';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='jaffe';format='Nevin';extFilter='*'}
	[pscustomobject]@{name='jefferis';format='AmiraMesh';extFilter='*'}
	[pscustomobject]@{name='jefferis+diadem';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='kawaguchi';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='kilb';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='korngreen';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='korte';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='kubota';format='NeurolucidaDAT';extFilter='*'}
#	[pscustomobject]@{name='larkman';format='???';extFilter='*'}
	[pscustomobject]@{name='lee';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='lewis';format='Eutectics';extFilter='*'}
	[pscustomobject]@{name='luebke';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='lu_lichtman';format='SWC';extFilter='*'}
	[pscustomobject]@{name='lu_lichtman+diadem';format='SWC';extFilter='*'}
	[pscustomobject]@{name='mailly';format='NeurolucidaASCClassic';extFilter='asc_classic'}
	[pscustomobject]@{name='mailly';format='NeurolucidaASC';extFilter='asc_modern'}
	[pscustomobject]@{name='mailly';format='NeurolucidaDAT';extFilter='dat'}
	[pscustomobject]@{name='markram';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='martin';format='NeuronHOC';extFilter='*'}
	[pscustomobject]@{name='martone';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='masland';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='mcbain';format='NeurolucidaASC';extFilter='asc'}
	[pscustomobject]@{name='mcbain';format='NeurolucidaDAT';extFilter='DAT'}
	[pscustomobject]@{name='meyer';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='miller';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='mizrahi';format='NeurolucidaASC';extFilter='asc'}
	[pscustomobject]@{name='mizrahi';format='NeurolucidaDAT';extFilter='DAT'}
	[pscustomobject]@{name='monyer';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='nolan';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='nusser';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='petrosini';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='pierce';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='poorthuis';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='povysheva';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='pozzo-miller';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='rhode';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='rose';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='smith';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='spruston';format='NeuronHOC';extFilter='*'}
	[pscustomobject]@{name='staiger';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='stevens';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='sun_prince';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='svoboda';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='timofeev';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='topolnik';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='turner';format='SWC';extFilter='*'}
	[pscustomobject]@{name='vervaeke';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='vuksic';format='NeurolucidaDAT';extFilter='NRX'}
	[pscustomobject]@{name='vuksic';format='NeurolucidaASC';extFilter='asc'}
	[pscustomobject]@{name='wearne_hof';format='SWC';extFilter='*'}
	[pscustomobject]@{name='wittner';format='NeurolucidaASC';extFilter='*'}
	[pscustomobject]@{name='wittner+ascoli+buzsaki';format='SWC';extFilter='*'}
	[pscustomobject]@{name='yu';format='NeurolucidaDAT';extFilter='*'}
	[pscustomobject]@{name='yuste';format='NeurolucidaASC';extFilter='*'}
)

# write commands for all tests. 
# We must setup archives individually because we don't want to rely on auto-detect to determine file format. Auto-detect is itself part of the test

foreach($archive in $archive_list)
{
	Ninja-Add-NeuroMorpho-Test-Archive $NinjaBuildFile $NeuroMorphoArchive $archive.name $archive.format $archive.extFilter
}


Start-Process -NoNewWindow -FilePath $NinjaExecutable
