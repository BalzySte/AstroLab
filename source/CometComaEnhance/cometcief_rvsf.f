      program cometcief_rvsf
c
c     This code enhances an image using radially variable spatial
c     filtering with new keywords added to the header
c     This allows fractional pixel values for the location of the
c     nucleus (optocenter)
c
c     written by Nalin Samarasinha (updated Dec 06, 2013)
c     original algorithm is by Steve Larson
c
      character*72 errmsg,imold,imnew
      real aarray(10800000),barray(10800000)
      real avect(4096)
      real a0(10,10)
      real twopi
      real xnuc,ynuc
      real sumpix,avepix,renorm
      real a,b,en
      real rho,sumedg,sumcrn
      real am,an
c
      integer axleno(7)
      integer nxnuc,nynuc,xint,xfin,yint,yfin
c
      integer lrow,lcol
      integer ier,imo,imn,naxis,dtype
      integer lpixel
      integer numlog,num
      integer ii,jj,iii,jjj
c
      twopi=4.0*asin(1.0)
      write(*,'("give input image name:",$)')
      read(*,*) imold
      write(*,'("transform image to log10 space before filtering?",$)')
      write(*,'("yes=1, no=any other integer")')
      read(*,*) numlog
      write(*,'("give output image name:",$)')
      read(*,*) imnew
c
c     open the input image
c
      call imopen(imold,1,imo,ier)
       if(ier.ne.0) goto 999
      call imgsiz(imo,axleno,naxis,dtype,ier)
       if(ier.ne.0) goto 999
c
c     read the required pixels to be processed of the input image
c
      write(*,'("give x,y pixel coordinates of the nucleus")')
      write(*,'("(to first decimal):",$)')
      read(*,*) xnuc,ynuc
c
c     approximate nucleus coordinates as nearest integers
c
      nxnuc=nint(xnuc)
      nynuc=nint(ynuc)
c
      write(*,'("give first & last x-axis pixels to be processed:",$)') 
      read(*,*) xint,xfin
      write(*,'("give first & last y-axis pixels to be processed:",$)')
      read(*,*) yint,yfin
      call imgs2r(imo,aarray,xint,xfin,yint,yfin,ier)
       if(ier.ne.0) goto 999
c
c     give the kernel values
c
      write(*,'("give kernal A term:",$)')
      read(*,*) a
      write(*,'("give kernal B term:",$)')
      read(*,*) b
      write(*,'("give kernal N term:",$)')
      read(*,*) en
c
      xnuc=xnuc-float(xint)+1.0
      ynuc=ynuc-float(yint)+1.0
      lrow=xfin-xint+1
      lcol=yfin-yint+1
c
      lpixel=lrow*lcol
c
c     renormalize the image (average pixel value=100.0)
c
      sumpix=0.0
      do 100 i=1,lpixel
      sumpix=sumpix+aarray(i)
  100 continue
      avepix=sumpix/float(lpixel)
      renorm=100.0/avepix
c
      do 101 i=1,lpixel
      aarray(i)=aarray(i)*renorm
  101 continue
c
      if(numlog.ne.1) goto 105
c
c     do not take log if any pixel is near zero or less
c
      do 102 i=1,lpixel
      if(aarray(i).lt.1.0e-15) goto 104
  102 continue
c
      do 103 i=1,lpixel
      aarray(i)=alog10(aarray(i))
  103 continue
      goto 105
c
  104 numlog=0
c
c     create a new image (x,y) and open it
c
  105  call imopnc(imnew,imo,imn,ier)
        if(ier.ne.0) goto 999
       call imakwc(imn,'INP_IM',imold,'input image name',ier)
       if(ier.ne.0) goto 999
       call imakwr(imn,'NUC-X',xnuc,'optocenter X pixel value',ier)
        if(ier.ne.0) goto 999
       call imakwr(imn,'NUC-Y',ynuc,'optocenter Y pixel value',ier)
        if(ier.ne.0) goto 999
       if (numlog.eq.1) then
       call imakwi(imn,'numlog',numlog,'numlog=1 => logrithmic image'
     : ,ier)
       if(ier.ne.0) goto 999
       endif
       if (numlog.ne.1) then
       call imakwi(imn,'numlog',numlog,'numlog ne 1 => 
     :  no logrithmic image',ier)
       if(ier.ne.0) goto 999
       endif
       call imakwr(imn,'a',a,'coeficient a',ier)
        if(ier.ne.0) goto 999
       call imakwr(imn,'b',b,'coeficient b',ier)
        if(ier.ne.0) goto 999
       call imakwr(imn,'n',en,'index n',ier)
        if(ier.ne.0) goto 999
c
c     apply radially variable filtering to the image
c
       sumedg=0.0
       sumcrn=0.0
      do 800 i=1,lcol
       do 700 j=1,lrow
       num=lrow*(i-1)+j
c
c     do sub-pixel sampling in m and n (10x10 subpixels)
c
       do 602 m=1,10
       am=-5.5e-01+float(m)*1.0e-01
       do 601 n=1,10
       an=-5.5e-01+float(n)*1.0e-01
       rho=sqrt((float(j)-xnuc+am)**2.+(float(i)-ynuc+an)**2.)
       a0(m,n)=a+(b*(rho**en))
  601  continue
  602  continue
c
c     carry out sum over edge pixels, sumedg
c
      do 640 m=1,10
      do 630 n=1,10
       do 620 ii=-1,1,1
        iii=i+nint(float(ii)*a0(m,n))
        if(iii.lt.1) goto 620
        if(iii.gt.lcol) goto 620
        do 610 jj=-1,1,1
        jjj=j+nint(float(jj)*a0(m,n))
        if(jjj.lt.1) goto 610
        if(jjj.gt.lrow) goto 610
        if((iii-i).eq.0.and.(jjj-j).ne.0) goto 605
        if((jjj-j).eq.0.and.(iii-i).ne.0) then
        goto 605
        else
        goto 610
        endif
  605   sumedg=sumedg+aarray(lrow*(iii-1)+jjj)
  610   continue
  620  continue
  630 continue
  640 continue
      sumedg=sumedg*1.0e-02
c
c     carry out sum over corner pixels, sumcrn
c
      do 680 m=1,10
      do 670 n=1,10
       do 660 ii=-1,1,2
        iii=i+nint(float(ii)*a0(m,n))
        if(iii.lt.1) goto 660
        if(iii.gt.lcol) goto 660
        do 650 jj=-1,1,2
        jjj=j+nint(float(jj)*a0(m,n))
        if(jjj.lt.1) goto 650
        if(jjj.gt.lrow) goto 650
        sumcrn=sumcrn+aarray(lrow*(iii-1)+jjj)
  650   continue
  660  continue
  670 continue
  680 continue
      sumcrn=sumcrn*1.0e-02
c       
       barray(num)=1024.0*aarray(num)-192.0*sumedg-64.0*sumcrn
       sumedg=0.0
       sumcrn=0.0
  700  continue
  800 continue
c
      do 840 i=1,lcol
       do 820 j=1,lrow
       avect(j)=barray(lrow*(i-1)+j)
  820  continue
       call impl2r(imn,avect,i,ier)
        if(ier.ne.0) goto 999
  840 continue
c
c     close the input image
c
      call imclos(imo,ier)
       if(ier.ne.0) goto 999
c     close the output image
      call imclos(imn,ier)
       if(ier.ne.0) goto 999
c
  900 format(1x,i5,3(1pd11.3))
c
      stop
c
c     print error message
c
  999 call imemsg(ier,errmsg)
      write(*,'("error:",a80)') errmsg
      stop
      end
