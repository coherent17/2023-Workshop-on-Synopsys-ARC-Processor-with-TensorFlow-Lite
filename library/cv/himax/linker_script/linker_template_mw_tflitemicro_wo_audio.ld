#define __ASSEMBLY__
#include <target_mem_config.h>

MEMORY {
#if (ICCM0_SIZE != 0)
	ICCM0 :		ORIGIN = ICCM0_START,	LENGTH = ICCM0_SIZE
#endif
#if (ICCM1_SIZE != 0)
	ICCM1 :		ORIGIN = ICCM1_START,	LENGTH = ICCM1_SIZE
#endif
#if (DCCM_SIZE !=0)
	DCCM :		ORIGIN = DCCM_START,	LENGTH = DCCM_SIZE
#endif
#if (EXT_RAM_SIZE != 0)
  #SYSTEM0 : ORIGIN = EXT_RAM_APPDATA_START, LENGTH = 0xAFDE0
   SYSTEM0 : ORIGIN = EXT_RAM_APPDATA_START, LENGTH = (0x2011D670-EXT_RAM_APPDATA_START)
#endif
  XCCM :		ORIGIN = 0x90000000,	LENGTH = 32768
  YCCM :		ORIGIN = 0xA0000000,	LENGTH = 32768	
#ifdef FLASH_AS_SRAM   	
  FLASH_SECTOR : ORIGIN = FLASH_SECTOR_START,	LENGTH = 0x00300000
#endif  
}

ENTRY(_arc_start)

SECTIONS {

	GROUP : {
		.init_bootstrap:{
			*(.init_vector)
			*(.init_bootstrap)
		}
		.vector ALIGN(1024): {
			_f_vector = .;
      		*(.vector .vector.*)
			_e_vector = .;
		}
	} > ICCM1

	GROUP : {

		.text ALIGN(4): {
			_f_text = .;
			*(TYPE text)
			_e_text = .;
		}
 	} > ICCM1

#ifdef FLASH_AS_SRAM 	
 	 	GROUP : {
     .rodata ALIGN(4): {
         #ifdef TFLITE_MICRO_GOOGLE_PERSON
         		person_detect_model_data.o (TYPE rodata)
         #endif 
         #ifdef YOLO
         		yolo_PTQ_int8.o (TYPE rodata)
         #endif 
         #ifdef HX_VIP_YOMO_20210115_E_PANA
         		YOMO_E_PANA_20210115.o (TYPE rodata)
         #endif 
         #ifdef HX_VIP_YOMO_20210121_PBR_PANA
                YOMO_PBR_AoS96_PANA_20210121.o (TYPE rodata)
         #endif
         #ifdef HX_VIP_YOMO_DOOR_DETECT
                YOMO_DOOR_DETECT.o (TYPE rodata)
         #endif
         #ifdef HX_VIP_YOMO_AMR_ROI
				#ifdef TFLITE_MICRO_HIMAX_V15
         				PANA01_IOULoss05_borderValue128_YOMOv4.o (TYPE rodata)
         				YOMO_UFPR_AMR_CR_x1_3_t2_rotate5_aug_ori.o (TYPE rodata)
         		#endif
         		#ifdef TFLITE_MICRO_HIMAX_V24
         				PANA01_IOULoss05_borderValue128_YOMOv4_tf2_4.o (TYPE rodata)
         				YOMO_UFPR_AMR_CR_x1_3_t2_rotate5_aug_ori_tf2_4.o (TYPE rodata)
         		#endif
         #endif
            }
      } > FLASH_SECTOR 
#endif      

	GROUP BLOCK(4): {
		_f_bss_tensor_arena = .;
		.tensor_arena? ALIGN(16): {}
		_e_bss_tensor_arena = .;
	} > SYSTEM0
    
 	GROUP : { 
		.rodata  ALIGN(4): {
			_f_rodata = .;
			_fctors = .;
			*(.ctors*)
			_ectors = .;
			_fdtors = .;
			*(.dtors*)
			_edtors = .;
			_feh_frame = .;
			*(.eh_frame*)
			_eeh_frame = .;
			*(TYPE lit)
			_e_rodata = .;
		}
	} > SYSTEM0

   GROUP BLOCK(4): {
       .Xdata? : {}
   } > XCCM
   GROUP BLOCK(4): {
        .Ydata? : {}
   } > YCCM
   GROUP BLOCK(4): {
            .rodata_in_data?:{}
    } > DCCM

	GROUP : {
		.data ALIGN(8): {
			_f_data = .;
			_f_sdata = .;
			*(.sdata)
			_e_sdata = .;
			*(TYPE data)
		}
		.tls ALIGN(8): {
			*(.tls*)
			_e_data = .;
		}
	} > DCCM 

	GROUP (NOLOAD) : {
		.bss ALIGN(8): {
			_f_bss = .;
			*(TYPE bss)
			_e_bss = .;
		}
	} > DCCM
  
  GROUP : {
    .sdata?: {}
    .sbss?: {}
    .protobuf?: {}
    .Zdata?: {}
    .heap? ALIGN(8) SIZE(_HEAPSIZE): {}
    .stack ALIGN(8) SIZE(_STACKSIZE): {}
  } >DCCM

	_f_stack = ADDR(.stack);
	_e_stack = ADDR(.stack) + SIZEOF(.stack);
	_f_heap = ADDR(.heap);
	_e_heap = ADDR(.heap) + SIZEOF(.heap);

	_load_addr_text = LOADADDR(.text);
	_load_addr_rodata = LOADADDR(.rodata);
	_load_addr_data = LOADADDR(.data);
}