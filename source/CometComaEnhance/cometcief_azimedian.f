      program cometcief_azimedian
c
c     This code enhances an image using division by azimuthal median 
c     with new keywords added to the header
c     This allows fractional pixel values for the location of the
c     nucleus (optocenter)
c
c     The median values for each radius are printed in the respective 
c     profile_* file as (radius i in pixels, i*median, median)
c
c     written by Nalin Samarasinha (updated Dec 06, 2013)
c
      character*72 errmsg,imold,imintu,iminte,imnew,improf
      character*8 prof
      real aarray(10800000),avect(3600),bvect(3600),cvect(4096)
      real angcos(36000),angsin(36000)
      real twopi,xmax,ymax,angle,median
      real xdist,ydist,angthe
      real ai,floati,floatj
      real xnuc,ynuc
      real fmthet
      real sumpix,avepix,renorm
      real val
      integer axleno(7),axleni(7)
      integer nxnuc,nynuc,xint,xfin,yint,yfin,nramax,nramin,nrad
      integer nxymax,nxymin,ntheta,lrow,lcol,xpix,ypix,rpix,thpix
      integer mtheta,jnew
      integer ier,imo,imiun,imien,imn,naxis,dtype
      integer lpixel,nmid
c
      twopi=4.0*asin(1.0)
      write(*,'("give input image name:",$)')
      read(*,*) imold
      write(*,'("give unenhaced rho,theta image name:",$)')
      read(*,*) imintu
      write(*,'("give enhanced rho,theta image name:",$)')
      read(*,*) iminte
      write(*,'("give output image name:",$)')
      read(*,*) imnew
c
c     define the azimed profile
c
      prof='profile_' 
      improf=prof//imnew
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
c     create new intermediate images (rho,theta) and open them
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
      axleni(1)=ntheta
      axleni(2)=nrad
      do 10 i=3,7
       axleni(i)=1
   10 continue
      call imcrea(imintu,axleni,naxis,dtype,ier)
       if(ier.ne.0) goto 999
      call imopen(imintu,3,imiun,ier)
       if(ier.ne.0) goto 999
c
      call imakwc(imiun,'INP_IM',imold,'input image name',ier)
       if(ier.ne.0) goto 999
c
      call imcrea(iminte,axleni,naxis,dtype,ier)
       if(ier.ne.0) goto 999
      call imopen(iminte,3,imien,ier)
       if(ier.ne.0) goto 999
c
      call imakwc(imien,'INP_IM',imold,'input image name',ier)
       if(ier.ne.0) goto 999
c
c     sort out the corresponding pixels for rho,theta image, 
c     do averaging for rows and write to the rho,theta image
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
      do 110 j=1,mtheta
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
  110 continue
c
      open(unit=11,file=improf,access='sequential',status='new')
      write(11,*) '  rho ',' rho*median ',' median'
c
      do 300 i=1,nrad
       floati=float(i)
       do 200 j=1,ntheta
        avect(j)=0.0
        do 140 ii=1,10
         ai=floati-0.55+(0.1*float(ii)) 
         do 120 jj=1,10
          jnew=10*(j-1)+jj
          xpix=nint(xnuc+(ai*angcos(jnew)))
          if(xpix.lt.1) goto 150
          if(xpix.gt.lrow) goto 150
          ypix=nint(ynuc+(ai*angsin(jnew)))
          if(ypix.lt.1) goto 150
          if(ypix.gt.lcol) goto 150
          avect(j)=avect(j)+aarray(lrow*(ypix-1)+xpix)
  120    continue
  140   continue
        avect(j)=0.01*avect(j)
        if(avect(j).lt.1.0e-05) avect(j)=-1.0e-05
        goto 200
  150   avect(j)=-1.0
  200  continue
c
       call impl2r(imiun,avect,i,ier)
        if(ier.ne.0) goto 999
c
c      calculate the azimuathal median
c       median=amedr(avect,ntheta)
c
c      determine the azimuthal median by an alternate way
c
       jjj=0
       do 210 j=1,ntheta
       bvect(j)=avect(j)
       if(bvect(j).lt.0.0) jjj=jjj+1
  210  continue
c
       do 230 j=2,ntheta
        val=bvect(j)
        jj=j
  220   jj=jj-1
        if(jj.eq.0) goto 225
        if(bvect(jj).le.val) goto 225
        bvect(jj+1)=bvect(jj)
        goto 220
  225   bvect(jj+1)=val
  230  continue
c
       nmid=int(aint(float(ntheta+jjj)/2.0))
c
c      this median value is the first value of the pair of
c      mid-pixel values in the sorted array when the effective
c      pixel number is an even number
c
       median=bvect(nmid)
c
       write(11,900) i,float(i)*median,median
c	 
       do 240 j=1,ntheta
        bvect(j)=avect(j)/(median+1.0e-6)
  240  continue
c
       call impl2r(imien,bvect,i,ier)
        if(ier.ne.0) goto 999
  300 continue
c
      close(unit=11,status='keep')
c
c     read the pixels of rho,theta images and close them
c
      call imgs2r(imiun,aarray,1,ntheta,1,nrad,ier)
       if(ier.ne.0) goto 999
      call imclos(imiun,ier)
       if(ier.ne.0) goto 999
c
      call imgs2r(imien,aarray,1,ntheta,1,nrad,ier)
       if(ier.ne.0) goto 999
      call imclos(imien,ier)
       if(ier.ne.0) goto 999
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
       floati=float(i)
       do 400 j=1,lrow
        floatj=float(j)
        cvect(j)=0.0
        do 340 ii=1,10
         ydist=floati+(0.1*float(ii))-0.55-ynuc
         do 320 jj=1,10
          xdist=floatj+(0.1*float(jj))-0.55-xnuc
          rpix=nint(sqrt(xdist**2.+ydist**2.))
          if(rpix.lt.1) goto 315
          if(rpix.gt.nrad) goto 315
          angthe=atan2(-xdist,ydist)
          if(angthe.lt.0.0) angthe=angthe+twopi
          if(angthe.gt.twopi) angthe=angthe-twopi
c
c     angle angthe here is the position angle and therefore in 
c     the new x,y image, it has to be measured counterclockwise
c     from the +y axis (hence the way how it is defined)
c
          thpix=nint(0.5+angthe*float(ntheta)/twopi)
          cvect(j)=cvect(j)+aarray(ntheta*(rpix-1)+thpix)
         goto 320
  315    cvect(j)=cvect(j)
  320    continue
  340   continue
        cvect(j)=0.01*cvect(j)
  400  continue
       call impl2r(imn,cvect,i,ier)
        if(ier.ne.0) goto 999
  500 continue
c
c     close the input image
c
      call imclos(imo,ier)
       if(ier.ne.0) goto 999
c
c     close the output image
c
      call imclos(imn,ier)
       if(ier.ne.0) goto 999
c
  900 format(1x,i5,3(1pd11.3))
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
