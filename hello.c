Skip to content
Search or jump to…

Pull requests
Issues
Marketplace
Explore
 
@brunovcordeiro 
Learn Git and GitHub without any code!
Using the Hello World guide, you’ll start a branch, write comments, and open a pull request.


4
02nanvix/utils
 Code Issues 10 Pull requests 0 Security Insights
utils/arch/mppa256.sh
@ppenna ppenna Enhancement: Flexible Output Parsing
fbd7378 on 13 Sep
128 lines (118 sloc)  3.04 KB
  
#
# MIT License
#
# Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

export K1_TOOLCHAIN_DIR="/usr/local/k1tools"

#
# Sets up development tools.
#
function setup_toolchain
{
	# Nothing to do.
	echo ""
}

#
# Builds system image.
#
function build
{
	local image=$1
	local bindir=$2
	local binary=$3
	local iobin=$binary-k1bio
	local nodebin=$binary-k1bdp

	$K1_TOOLCHAIN_DIR/bin/k1-create-multibinary \
		--boot $bindir/$iobin                   \
		--clusters $bindir/$nodebin             \
		-T $image -f
}

#
# Runs a binary in the platform.
#
function run
{
	local image=$1
	local bindir=$2
	local bin=$3
	local target=$4
	local variant=$5
	local mode=$6
	local timeout=$7
	local args=$8
	local execfile=""

	case $variant in
		"all")
			          execfile="\
                --exec-file=IODDR0:$bindir/$bin-k1bio    \
                --exec-file=IODDR1:$bindir/$bin-k1bio    \
                --exec-file=Cluster0:$bindir/$bin-k1bdp  \
                --exec-file=Cluster1:$bindir/$bin-k1bdp  \
                --exec-file=Cluster2:$bindir/$bin-k1bdp  \
                --exec-file=Cluster3:$bindir/$bin-k1bdp  \
                --exec-file=Cluster4:$bindir/$bin-k1bdp  \
                --exec-file=Cluster5:$bindir/$bin-k1bdp  \
                --exec-file=Cluster6:$bindir/$bin-k1bdp  \
                --exec-file=Cluster7:$bindir/$bin-k1bdp  \
                --exec-file=Cluster8:$bindir/$bin-k1bdp  \
                --exec-file=Cluster9:$bindir/$bin-k1bdp  \
                --exec-file=Cluster10:$bindir/$bin-k1bdp \
                --exec-file=Cluster11:$bindir/$bin-k1bdp \
                --exec-file=Cluster12:$bindir/$bin-k1bdp \
                --exec-file=Cluster13:$bindir/$bin-k1bdp \
                --exec-file=Cluster14:$bindir/$bin-k1bdp \
                --exec-file=Cluster15:$bindir/$bin-k1bdp \
            "
			;;
		"iocluster")
			execfile="--exec-file=IODDR0:$bindir/$bin-k1bio"
			;;
		"ccluster")
			execfile="--exec-file=Cluster0:$bindir/$bin-k1bdp"
			;;
	esac

	if [ $mode == "--debug" ];
	then
		$K1_TOOLCHAIN_DIR/bin/k1-jtag-runner \
			--gdb                            \
			--multibinary=$image             \
			$execfile                        \
			-- $args
	else
		if [ ! -z $timeout ];
		then
			timeout --foreground $timeout        \
			$K1_TOOLCHAIN_DIR/bin/k1-jtag-runner \
				--multibinary=$image             \
				$execfile                        \
				-- $args                         \
			|& tee $OUTFILE
			line=$(cat $OUTFILE | tail -1 )
			if [[ "$line" = *"powering off"* ]] || [[ $line == *"halting"* ]];
			then
				echo "Succeed !"
			else
				echo "Failed !"
				return -1
			fi
		else
			$K1_TOOLCHAIN_DIR/bin/k1-jtag-runner \
				--multibinary=$image             \
				$execfile                        \
				-- $args
		fi
	fi
}

#
# Runs a binary in the platform (simulator).
#
function run_sim
{
	local bin=$1
	local args=$2

	$K1_TOOLCHAIN_DIR/bin/k1-cluster \
		--mboard=$BOARD           \
		--march=$ARCH             \
		--bootcluster=node0       \
		-- $bin $args
}
© 2019 GitHub, Inc.
Terms
Privacy
Security
Status
Help
Contact GitHub
Pricing
API
Training
Blog
About
