#define _WIN32_WINNT 0x0500
#define _WIN32_WINDOWS 0x0500
#define WINVER 0x0500

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lusb0_usb.h"
#include "qled.h"

#include <QMouseEvent>

#include <windows.h>
#include <Dbt.h>
#include <winuser.h>


#define VND_GET_LEDS 0x10
#define VND_SET_LED  0x11

#define VALID_VUD_DEVICE(vid, pid)             \
   ((((vid) == 0x2544) || ((vid) == 0x10C4))   \
    && (((pid) == 0x0001) || ((pid) == 0x0008) || ((pid) == 0x000C)))

// This GUID is defined by Microsoft
static const GUID GUID_DEVINTERFACE_USB_DEVICE =
{ 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED} };

static usb_dev_handle *pDevH = NULL;  // USB device handle
static struct usb_device *pDev;
static struct usb_bus *pBus;
static uint8_t leds;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  Led4 = new Led( this, 4, 20,  20, 50, 50 );
  Led3 = new Led( this, 3, 80,  20, 50, 50 );
  Led2 = new Led( this, 2, 140, 20, 50, 50 );
  Led1 = new Led( this, 1, 200, 20, 50, 50 );
  Led0 = new Led( this, 0, 260, 20, 50, 50 );

  ConnectUSBDevice();

  // Register for USB device arrival/removal events.
  DEV_BROADCAST_DEVICEINTERFACE_W dbi;
  ZeroMemory( &dbi, sizeof(dbi) );
  dbi.dbcc_size       = sizeof(dbi);
  dbi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
  dbi.dbcc_classguid  = GUID_DEVINTERFACE_USB_DEVICE;
  RegisterDeviceNotification((HANDLE)winId(), &dbi, DEVICE_NOTIFY_WINDOW_HANDLE);
}

MainWindow::~MainWindow()
{
  DisconnectUSBDevice();
  delete ui;
}

void MainWindow::LedClicked( int ledNo )
{
  QString s;

  VendorUniqueSetLedCmd( ledNo, leds & (1<<ledNo) ? 0 : 1 );
  UpdateLedsOnForm();
  s = "Toggled Led ";
  s.append( ledNo + '0' );
  ui->statusBar->showMessage( s );
}

void MainWindow::VendorUniqueSetLedCmd( int ledNo, int on )
{
  if ( pDevH )
  {
    usb_control_msg(
                  pDevH,            // Device handle
                  USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
                                    // bmRequestType
                  VND_SET_LED,      // bRequest
                  on,               // wValue
                  ledNo,            // wIndex
                  NULL,             // char *pBuffer
                  0x0000,           // wLength
                  1000 );           // Timeout (ms)
  }
}

uint8_t MainWindow::VendorUniqueGetLedsCmd( void )
{
  uint8_t leds = 0;

  if ( pDevH )
  {
    usb_control_msg(
                  pDevH,            // Device handle
                  USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
                                    // bmRequestType
                  VND_GET_LEDS,     // bRequest
                  0,                // wValue
                  0,                // wIndex
                  (char*)&leds,     // char *pBuffer
                  1,                // wLength
                  1000 );           // Timeout (ms)
  }

  return leds;
}

void MainWindow::UpdateLedsOnForm( void )
{
  leds = VendorUniqueGetLedsCmd();

  Led0->setValue( leds & 1 );
  Led1->setValue( leds & 2 );
  Led2->setValue( leds & 4 );
  Led3->setValue( leds & 8 );
  Led4->setValue( leds & 16 );
}

void MainWindow::ConnectUSBDevice( void )
{
  if ( pDevH == NULL )
  {
    usb_find_busses();
    usb_find_devices();

    // Enumerate USB devices ...
    for ( pBus = usb_get_busses(); pBus; pBus = pBus->next)
    {
      for ( pDev = pBus->devices; pDev; pDev = pDev->next)
      {
        if (VALID_VUD_DEVICE(pDev->descriptor.idVendor, pDev->descriptor.idProduct))
        {
          pDevH = usb_open( pDev );
          if ( pDevH )
          {
            if ( usb_set_configuration(
                      pDevH, pDev->config->bConfigurationValue ) != 0 )
            {
              usb_close( pDevH );
              pDevH = NULL;
              goto found;
            }

            if ( pDev->descriptor.idProduct == 0x0001 )
            {
              if ( usb_claim_interface( pDevH, 0 ) != 0 )
              {
                usb_close( pDevH );
                pDevH = NULL;
                goto found;
              }
            }

            if ( pDev->descriptor.idProduct == 0x0008 )
            {
              if ( usb_claim_interface( pDevH, 0 ) != 0 )
              {
                usb_close( pDevH );
                pDevH = NULL;
                goto found;
              }
            }
          }
          goto found;
        }
      }
    }

found:
    if ( pDevH == NULL )
    {
      ui->statusBar->showMessage( "No USB device found" );
    }
    else
    {
      ui->statusBar->showMessage( "EFM32 Vendor Unique Device attached, click on LED's to toggle" );
      leds = VendorUniqueGetLedsCmd();
      UpdateLedsOnForm();
    }
  }
}

void MainWindow::DisconnectUSBDevice( void )
{
  if ( pDevH )
  {
    usb_set_configuration( pDevH, 0 );
    usb_release_interface( pDevH, 0 );
    usb_close( pDevH );
    pDevH = NULL;
  }
}

bool MainWindow::winEvent(MSG *msg, long *result)
{
  int vid, pid;
  bool arrival;
  char tmp[100];
  Q_UNUSED( result );
  DEV_BROADCAST_DEVICEINTERFACE *x;

  if (msg->message == WM_DEVICECHANGE)
  {
    arrival = false;
    switch (msg->wParam)
    {
      case DBT_DEVICEARRIVAL:
        arrival = true;
      case DBT_DEVICEREMOVECOMPLETE:
        x = (DEV_BROADCAST_DEVICEINTERFACE*)msg->lParam;
        vid = QString::fromWCharArray(x->dbcc_name).mid(12,4).toInt(NULL, 16);
        pid = QString::fromWCharArray(x->dbcc_name).mid(21,4).toInt(NULL, 16);

        if (VALID_VUD_DEVICE(vid, pid))
        {
          if (arrival)
          {
            ConnectUSBDevice();
          }
          else
          {
            pDevH = NULL;
            UpdateLedsOnForm();
            sprintf( tmp, "USB device with VID/PID 0x%04X/0x%04X removed", vid, pid);
            ui->statusBar->showMessage(tmp);
          }
        }
        break;
    }
  }
  return false;
}

void Led::mousePressEvent( QMouseEvent *e )
{
  if ( e->buttons() & Qt::LeftButton )
  {
    parentForm->LedClicked( this->ledNo );
  }
}
