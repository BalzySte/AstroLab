      program cometcief_inverserho
c
c     This code enhances an image using division by one over rho
c     with new keywords added to the header
c     This allows fractional pixel values for the location of the
c     nucleus (optocenter)
c     Division by one over rho (actually multiplication by rho)
c     is carried out prior to generating the rho,theta images
c
c     written by Nalin Samarasinha (updated Dec 06, 2013)
c
      character*72 errmsg,imold,imintu,iminte,imnew
      real aarray(10800000),barray(10800000),avect(3600),bvect(4096)
      real angcos(36000),angsin(36000)
      real twopi,xmax,ymax,angle,valu,rho
      real x,y,xnew,ynew,floati,ai
      real xnuc,ynuc
      real fmthet
      real sumpix,avepix,renorm
      integer axleno(7),axleni(7)
      integer xint,xfin,yint,yfin,nramax,nramin,nrad
      integer nxymax,nxymin,ntheta,lrow,lcol,xpix,ypix
      integer ier,imo,imiun,imien,imn,naxis,dtype
      integer jx,iy,mtheta,jnew
      integer nxnuc,nynuc
      integer lpixel
c
      twopi=4.0*asin(1.0)
      write(*,'("give input image name:",$)')
      read(*,*) imold
      write(*,'("give unenhanced rho,theta image name:",$)')
      read(*,*) imintu
      write(*,'("give enhanced rho,theta image name:",$)')
      read(*,*) iminte
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
      write(*,'("give x,y pixel coordinates of the nucleus:",$)')
      write(*,'("(to the first decimal):",$)')
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
c     create new intermediate (rho,theta) images and open them
c
      xmax=amax0((xfin-nxnuc),(nxnuc-xint))
      ymax=amax0((yfin-nynuc),(nynuc-yint))
      nramax=nint(sqrt(xmax**2.+ymax**2.))
      nramin=min0((xfin-nxnuc),(nxnuc-xint),(yfin-nynuc),(nynuc-yint))
      write(*,901) nramax
      write(*,902) nramin
      write(*,'("give the number of pixels wanted in rho axis")')
      write(*,'("(must be smaller than radius of the")')
      write(*,'("largest incomplete circle):",$)')
      read(*,*) nrad
      nxymax=nint(twopi*sqrt(xmax**2.+ymax**2.))
      nxymin=nint(twopi*amin0((xfin-nxnuc),(nxnuc-xint),(yfin-nynuc),
     :(nynuc-yint)))
      write(*,903) nxymax
      write(*,904) nxymin
      write(*,'("give the number of pixels wanted in theta axis")')
      write(*,'("(currently set to a maximum of 3600 pixels):",$)')
      read(*,*) ntheta
      mtheta=ntheta*10
      fmthet=float(mtheta)
c
c     sort out the corresponding lengths for the rows and columns 
c
      xnuc=xnuc-float(xint)+1.0
      ynuc=ynuc-float(yint)+1.0
c     lrow is the number of pixels along the x axis
      lrow=xfin-xint+1
c     lcol is the number of pixels along the y axis
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
c     write the unenhanced (rho,theta) image
c
      do 160 j=1,mtheta
       angle=(float(j)-0.5)*twopi/fmthet
c
c     angle here is measured counterclockwise from the +x axis
c     however, as the position angle is measured from the +y
c     axis in a counterclockwise sense, angcos and angsin are
c     defined as the following - thus resulting in a rho,theta
c     image where theta is indeed the position angle
c
       angcos(j)=-sin(angle)
       angsin(j)=cos(angle)
  160 continue
c
      axleni(1)=ntheta
      axleni(2)=nrad
      do 170 i=3,7
       axleni(i)=1
  170 continue
      call imcrea(imintu,axleni,naxis,dtype,ier)
       if(ier.ne.0) goto 999
      call imopen(imintu,3,imiun,ier)
       if(ier.ne.0) goto 999
c
      call imakwc(imiun,'INP_IM',imold,'input image name',ier)
       if(ier.ne.0) goto 999
c
      do 200 i=1,nrad
       floati=float(i)
       do 190 j=1,ntheta
        avect(j)=0.0
        do 174 ii=1,10
         ai=floati-0.55+(0.1*float(ii))
         do 172 jj=1,10
          jnew=10*(j-1)+jj
          xpix=nint(xnuc+(ai*angcos(jnew)))
          if(xpix.lt.1) goto 180
          if(xpix.gt.lrow) goto 180
          ypix=nint(ynuc+(ai*angsin(jnew)))
          if(ypix.lt.1) goto 180
          if(ypix.gt.lcol) goto 180
          avect(j)=avect(j)+aarray(lrow*(ypix-1)+xpix)
  172    continue
  174   continue
        avect(j)=0.01*avect(j)
        if(avect(j).lt.1.0e-05) avect(j)=-1.0e-05
        goto 190
  180   avect(j)=-1000000.0
  190  continue
c
       call impl2r(imiun,avect,i,ier)
        if(ier.ne.0) goto 999
  200 continue
c
c     read the pixels of unenhanced (rho,theta) image and close it
c
      call imgs2r(imiun,barray,1,ntheta,1,nrad,ier)
       if(ier.ne.0) goto 999
      call imclos(imiun,ier)
       if(ier.ne.0) goto 999
c
c     multiply aarray pixels by rho
c
      do 220 i=1,lcol
      iy=i
       do 210 j=1,lrow
       jx=j
       rho=0.0
       valu=aarray(lrow*(iy-1)+jx)
       x=xnuc-float(jx)
       y=ynuc-float(iy)
        do 204 ii=1,10
        xnew=x-0.55+(0.1*float(ii))
         do 202 jj=1,10
         ynew=y-0.55+(0.1*float(jj))
         rho=rho+sqrt((xnew*xnew)+(ynew*ynew))
  202    continue
  204   continue
        rho=rho*0.01
        valu=valu*rho
        aarray(lrow*(iy-1)+jx)=valu
  210  continue
  220 continue
c
c     create a new image (x,y) and open it
c
      call imopnc(imnew,imo,imn,ier)
       if(ier.ne.0) goto 999
      call imakwc(imn,'INP_IM',imold,'input image name',ier)
       if(ier.ne.0) goto 999
      call imakwr(imn,'NUC-X',xnuc,'optocenter X pixel value',ier)
       if(ier.ne.0) goto 999
      call imakwr(imn,'NUC-Y',ynuc,'optocenter Y pixel value',ier)
       if(ier.ne.0) goto 999
      call imakwi(imn,'ENH-RAD',nrad,'diameter=1+(2*radius)',ier)
       if(ier.ne.0) goto 999
c
c     sort out the corresponding pixels for output image and
c     write them to the output image
c
      do 500 i=1,lcol
       do 400 j=1,lrow
        bvect(j)=aarray(lrow*(i-1)+j)
  400  continue
       call impl2r(imn,bvect,i,ier)
        if(ier.ne.0) goto 999
  500 continue
c
c     close the output image
c
      call imclos(imn,ier)
       if(ier.ne.0) goto 999
c
c     write the enhanced (rho,theta) image
c
      do 600 j=1,mtheta
       angle=(float(j)-0.5)*twopi/fmthet
c
c     angle here is measured counterclockwise from the +x axis
c     however, as the position angle is measured from the +y
c     axis in a counterclockwise sense, angcos and angsin are
c     defined as the following - thus resulting in an rho,theta 
c     image where theta is indeed the position angle
c
       angcos(j)=-sin(angle)
       angsin(j)=cos(angle)
  600 continue
c
      axleni(1)=ntheta
      axleni(2)=nrad
      do 610 i=3,7
       axleni(i)=1
  610 continue
      call imcrea(iminte,axleni,naxis,dtype,ier)
       if(ier.ne.0) goto 999
      call imopen(iminte,3,imien,ier)
       if(ier.ne.0) goto 999
c
      call imakwc(imien,'INP_IM',imold,'input image name',ier)
       if(ier.ne.0) goto 999
c
      do 800 i=1,nrad
       floati=float(i)
       do 700 j=1,ntheta
        avect(j)=0.0
        do 640 ii=1,10
         ai=floati-0.55+(0.1*float(ii))
         do 620 jj=1,10
          jnew=10*(j-1)+jj
          xpix=nint(xnuc+(ai*angcos(jnew)))
          if(xpix.lt.1) goto 650
          if(xpix.gt.lrow) goto 650
          ypix=nint(ynuc+(ai*angsin(jnew)))
          if(ypix.lt.1) goto 650
          if(ypix.gt.lcol) goto 650
          avect(j)=avect(j)+aarray(lrow*(ypix-1)+xpix)
  620    continue
  640   continue
        avect(j)=0.01*avect(j)
        if(avect(j).lt.1.0e-05) avect(j)=1.0e-05
        goto 700
  650   avect(j)=-1000000.0
  700  continue
c
       call impl2r(imien,avect,i,ier)
        if(ier.ne.0) goto 999
  800 continue
c
c     read the pixels of enhanced rho,theta image and close it
c
      call imgs2r(imien,aarray,1,ntheta,1,nrad,ier)
       if(ier.ne.0) goto 999
      call imclos(imien,ier)
       if(ier.ne.0) goto 999
c
c     close the input image
c
      call imclos(imo,ier)
       if(ier.ne.00) goto 999
c
  901 format(1x,"radius of the largest incomplete circle =",i5)
  902 format(1x,"radius of the largest complete circle =",i5)
  903 format(1x,"circumference of the largest incomplete circle =",i5)
  904 format(1x,"circumference of the largest complete circle =",i5)
c
      stop
c
c     print error message
c
  999 call imemsg(ier,errmsg)
      write(*,'("error:",a80)') errmsg
      stop
      end
