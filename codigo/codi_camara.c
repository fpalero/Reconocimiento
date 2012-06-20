// includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <pgm.h>
#include <ppm.h>
#include <vision.h>

#include "easywin.h"
// constants

#define VIDEO_DEVICE  "/dev/video0"
#define VIDEO_BUFFERS  1

#define VIDEO_ANCHO   320
#define VIDEO_ALTO    240

#define IMAGENES_CAPTURADAS      10
#define IMAGEN_ESPERA            1000000  // en microsegundos

// code

int main( int   argc, char *argv[] ){

  int                         fd, err, n, r, w, k;
  struct v4l2_fmtdesc         fmts;
  struct v4l2_streamparm      parm;
  struct v4l2_format          fmt;
  struct v4l2_requestbuffers  req;
  struct v4l2_buffer          buf;
  enum   v4l2_buf_type        type;
  void                       *vbuffers[VIDEO_BUFFERS];
  size_t                      vbuffers_lenght[VIDEO_BUFFERS];
  //char                        filename[512];
  //FILE                       *fi;
  gray                      **image_pgm;
  //pixel                     **image_ppm;
  int                         cols, rows;
  fd_set                      fds;
  struct timeval              tout;

  // open camera
  fd = open(VIDEO_DEVICE, O_RDWR  | O_NONBLOCK);
  if (fd == -1) {
    perror ("Open");
    exit(-1);
  } // end if
  fprintf(stderr, "Dispositivo %s abierto:\n", VIDEO_DEVICE);

  // formatos de video
  fprintf(stderr, " |-> Formatos de video disponibles:\n");
  // formatos aceptados por la camara
  bzero(&fmts, sizeof(fmts)); // poner a ceros
  fmts.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmts.index = 0;
  err = ioctl(fd, VIDIOC_ENUM_FMT, &fmts);
  while (err != -1) {
    fprintf(stderr, " |    |-> #%d: %s (0x%x %s)\n", 
            fmts.index, fmts.description, fmts.pixelformat, (fmts.flags & V4L2_FMT_FLAG_COMPRESSED) ? "comprimido" : "");
    fmts.index++;
    err = ioctl(fd, VIDIOC_ENUM_FMT, &fmts);
  } // end while


   // set framerate
  bzero(&parm, sizeof(parm)); // poner a ceros
  parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  parm.parm.capture.timeperframe.numerator = 1;
  parm.parm.capture.timeperframe.denominator = 30; // FPS
  if ((err = ioctl(fd, VIDIOC_S_PARM, &parm))<0) {
    perror("VIDIOC_S_PARAM");
    exit(-1);
  } // end if
  fprintf(stderr, " |-> FPS: %d\n", parm.parm.capture.timeperframe.denominator); 


  // set resolution & video format
  bzero(&fmt, sizeof(fmt)); // poner a ceros
  fmt.type           = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width  = VIDEO_ANCHO; // columnas 
  fmt.fmt.pix.height = VIDEO_ALTO; // filas
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG; // modo de video
  if ((err = ioctl(fd, VIDIOC_S_FMT, &fmt))<0) {
    perror("VIDIOC_S_FMT");
    exit(-1);
  } // end if
  fprintf(stderr, " |-> Tama�o: %dx%d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
  fprintf(stderr, " |-> Formato: MJPEG\n");
  
    // solicita buffers para el video
  bzero(&req, sizeof(req));
  req.count = VIDEO_BUFFERS;  // numero de buffer que se solicitaran
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  if ((err = ioctl(fd, VIDIOC_REQBUFS, &req))<0) {
    perror("VIDIOC_REQBUFS");
    exit(-1);
  } // end if

  // mapeo los buffers en memoria
  for (n = 0; n < req.count; n++) {
    bzero(&buf, sizeof(buf));
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index  = n;
    if ((err = ioctl(fd, VIDIOC_QUERYBUF, &buf))<0) {
      perror("VIDIOC_QUERYBUF");
      exit(-1);
    } // end if FORMATO DE LOS BUFFERS
    vbuffers_lenght[n] = buf.length;
    vbuffers[n] = mmap(NULL,
                        buf.length,
                        PROT_READ | PROT_WRITE /* required */,
                        MAP_SHARED /* recommended */,
                        fd, buf.m.offset);
    if (vbuffers[n] == MAP_FAILED) {
      perror("mmap");
      exit(-1);
    } // end if
  } // end for
  fprintf(stderr, " |-> Buffers mapeados: %d\n", req.count);



  // encola los buffers en el driver de la camara BUFFERS VACIOS
  for (n=0; n< VIDEO_BUFFERS; n++) {
    bzero(&buf, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = n;
    if ((err = ioctl(fd, VIDIOC_QBUF, &buf))<0) {
      perror("VIDIOC_QBUF");
      exit(-1);
    } // end if
	
  } // end for

  //activa captura de video
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if ((err = ioctl (fd, VIDIOC_STREAMON, &type))<0) {
    perror("VIDIOC_STREAMON");
    exit(-1);
  } // end if
  fprintf(stderr, "Video activo.\n");


  eaw_init();
  
  w = eaw_new_window("Prac2", 800, 600);
  n=0;
  // captura imagenes
  do{
    
    // leer imagen
    do {
	
      // espera a que llegue una imagen con timeout
      FD_ZERO (&fds);       // conjunto vacio
      FD_SET (fd, &fds);    // annade fd
      tout.tv_sec = 3;        // Timeout de 3 segundos
      tout.tv_usec = 0;
      r = select (fd + 1, &fds, NULL, NULL, &tout);  // espera
      // control de errores
      if (r == -1) {
        if (errno == EINTR)
          continue;
        perror("select");
        exit(-1);
      } // end if
      if (r == 0) {
        fprintf (stderr, "Imagen timeout.\n");
        exit(-1);
      } // end if

      // desencola el buffer
      bzero(&buf, sizeof(buf));
      buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      if ((err = ioctl(fd, VIDIOC_DQBUF, &buf))<0) {
        switch (errno) {
        case EAGAIN:
          // nothing yet
          continue;
        case EIO:
          // se puede ignorar EIO
        default:
          perror("VIDIOC_DQBUF");
          exit(-1);
        } // end switch
      } // end if
      break;
    } while (1);
    //fprintf(stderr, "Captura %02d Ok.", n);

    // convert to pgm and save
    image_pgm = sdv_jpeg_to_pgm(vbuffers[buf.index], &cols, &rows);
    if (!image_pgm) {
      fprintf(stderr, "Bad JPG image.\n");
    } else {
	if(n>0)
		eaw_draw_image(w, image_pgm, cols, rows);
	pgm_freearray(image_pgm, rows);
    } // end if/else

    // devolver el buffer al driverec
    if ((err = ioctl(fd, VIDIOC_QBUF, &buf))<0) {
      perror("VIDIOC_QBUF");
      exit(-1);
    } // end if

    k = eaw_wait_a_key(5) & 0xFF;
    n++;
  }while( k!= 'q'); // end do



  // Cerrando ventana y video
  eaw_close_window(w);

  // detener el video
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if ((err = ioctl (fd, VIDIOC_STREAMOFF, &type))<0) {
    perror("VIDIOC_STREAMOFF");
    exit(-1);
  } // end if

  // unmap buffers
  for (n=0; n<VIDEO_BUFFERS; ++n) {
    if (munmap (vbuffers[n], vbuffers_lenght[n])<0) {
      perror("munmap");
      exit(-1);
    } // end if
  } // end for

  // close camera
  close(fd);

  fprintf(stderr, "Dispositivo %s cerrado.\n", VIDEO_DEVICE);

  exit(0);

} // end main()
