#! /bin/bash

set -e
# command line stuff
if [ "$1" == "" ]; then
  echo "usage: $0 file_name"
  exit 1
fi
if [ ! -e $1 ]; then
  echo "file $1 not found"
  exit 1
fi
# format the file
rm -f x y
echo "#n     count        cpu wall" >x
echo "#- --------- ---------- ----" >>x
grep -v '#' $1 >y
./align_columns y >>x
echo "#- --------- ---------- ----" >>x
rm -f y
# data fit
a1="load 'x';n=x(:,1);t=log(x(:,3));idx=find(n>=40);n=n(idx);t=t(idx);N=100;"
a2="idx=find(mod(n,2)==0);str='even';"
a3="idx=find(mod(n,2)==1);str='odd';"
a4="X=[ones(size(n(idx))),n(idx)];"
if [[ $1 == *v1* ]]; then
  a5="w=pinv(X)*(t(idx)-log(n(idx)));T=exp(w(1))*N*exp(w(2))^N;"
  a6="printf('# %4s n: %.6e*n*%.6f^n\n',str,exp(w(1)),exp(w(2)));"
else
  a5="w=pinv(X)*t(idx);T=exp(w(1))*exp(w(2))^N;"
  a6="printf('# %4s n: %.6e*%.6f^n\n',str,exp(w(1)),exp(w(2)));"
fi
a7="printf('#   the expected time for n=%d is %.1f days\n',N,T/3600/24);"
echo "$a1$a2$a4$a5$a6$a7$a3$a4$a5$a6" | octave-cli >> x
mv -f x $1
chmod 400 $1
