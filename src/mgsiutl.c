/*
   ----------------------------------------------------------------------------
   | mgsi                                                                     |
   | Description: Service Integration Gateway                                 |
   | Author:      Chris Munt cmunt@mgateway.com                               |
   |                         chris.e.munt@gmail.com                           |
   | Copyright (c) 2002-2020 M/Gateway Developments Ltd,                      |
   | Surrey UK.                                                               |
   | All rights reserved.                                                     |
   |                                                                          |
   | http://www.mgateway.com                                                  |
   |                                                                          |
   | Licensed under the Apache License, Version 2.0 (the "License"); you may  |
   | not use this file except in compliance with the License.                 |
   | You may obtain a copy of the License at                                  |
   |                                                                          |
   | http://www.apache.org/licenses/LICENSE-2.0                               |
   |                                                                          |
   | Unless required by applicable law or agreed to in writing, software      |
   | distributed under the License is distributed on an "AS IS" BASIS,        |
   | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. |
   | See the License for the specific language governing permissions and      |
   | limitations under the License.                                           |
   |                                                                          |
   ----------------------------------------------------------------------------
*/


#include "mgsisys.h"
#include "mgsi.h"
#include "mgsidso.h"
#include "mgsiutl.h"


static char *month_list[] = { "Error", "January", "February", "March", 
				"April", "May", "June", "July", "August", 
				"September", "October", "November",
				"December" };


void * mg_malloc(unsigned long size, char *trace)
{
   void *p_mem;

#ifdef _WIN32
__try {
#endif

#if !defined(_WIN32)
   p_mem = malloc(size);
   if (p_mem)
      memset(p_mem, 0, size);
#else
   p_mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
#endif

   return p_mem;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_malloc: %x::%s", code, trace ? trace: "NULL");
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return NULL;
}
#endif

}


int mg_free(void *p_mem, char *trace)
{

#ifdef _WIN32
__try {
#endif

   if (!p_mem)
      return 0;

#if !defined(_WIN32)
      free(p_mem);
#else
      HeapFree(GetProcessHeap(), 0, p_mem);
#endif

   return 1;


#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_free: %x:%p:%s", code, p_mem, trace ? trace: "NULL");
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


void *mg_local_alloc(unsigned long size)
{
   void *p_mem;
#ifdef _WIN32
   p_mem = (void *) LocalAlloc(LPTR, size);
#else
   p_mem = malloc(size);
#endif
   return p_mem;
}


int mg_local_free(void *p_mem)
{
#ifdef _WIN32
   LocalFree((HLOCAL) p_mem);
#else
   free((void *) p_mem);
#endif
   return 0;
}


int mg_buf_init(LPMEMOBJ p_mem_obj, unsigned long size, unsigned long incr_size)
{
   int result;

#ifdef _WIN32
__try {
#endif

   if (size < MG_STMEM) {
      p_mem_obj->p_buffer = p_mem_obj->buffer;
      *(p_mem_obj->p_buffer) = '\0';
      result = 1;
   }
   else {

      p_mem_obj->p_buffer = (char *) mg_malloc(sizeof(char) * (size + 1), "mg_buf_init:1");

      if (p_mem_obj->p_buffer) {
         *(p_mem_obj->p_buffer) = '\0';
         result = 1;
      }
      else {
         result = 0;

         p_mem_obj->p_buffer = (char *) mg_malloc(sizeof(char) * 2, "mg_buf_init:2");

         if (p_mem_obj->p_buffer) {
            *(p_mem_obj->p_buffer) = '\0';
            size = 1;
         }
         else
            size = 0;
      }
   }

   p_mem_obj->size = size;
   p_mem_obj->incr_size = incr_size;
   p_mem_obj->curr_size = 0;

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_buf_init: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_buf_free(LPMEMOBJ p_mem_obj)
{

   if (p_mem_obj->size < MG_STMEM) {
      *(p_mem_obj->p_buffer) = '\0';
   }
   else {
      if (p_mem_obj->p_buffer) {

         mg_free((void *) p_mem_obj->p_buffer, "mg_buf_free:1");

      }
   }

   p_mem_obj->p_buffer = NULL;
   p_mem_obj->size = 0;
   p_mem_obj->incr_size = 0;
   p_mem_obj->curr_size = 0;

   return 1;
}



int mg_buf_cpy(LPMEMOBJ p_mem_obj, char *string)
{
   int result;
   unsigned long size;

   if (string)
      size = (unsigned long) strlen(string);
   else
      size = 0;

   result = mg_buf_cpy_ex(p_mem_obj, string, size);
   p_mem_obj->p_buffer[p_mem_obj->curr_size] = '\0';

   return result;
}


int mg_buf_cpy_ex(LPMEMOBJ p_mem_obj, char *buffer, unsigned long size)
{
   int result;
   unsigned long req_size, tsize, incr_size;

#ifdef _WIN32
__try {
#endif

   result = 1;

   if (buffer)
      req_size = size;
   else
      req_size = 0;

   if (req_size >= p_mem_obj->size) {
      tsize = p_mem_obj->size;
      incr_size = p_mem_obj->incr_size;
      while (req_size >= tsize)
         tsize = tsize + p_mem_obj->incr_size;

      mg_buf_free(p_mem_obj);
      result = mg_buf_init(p_mem_obj, tsize, incr_size);
   }
   if (result) {
      if (buffer && size > 0)
         memcpy((void *) p_mem_obj->p_buffer, (void *) buffer, size);
      else
         p_mem_obj->p_buffer[0] = '\0';

      p_mem_obj->curr_size = req_size;
   }

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_buf_cpy: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_buf_cat(LPMEMOBJ p_mem_obj, char *string)
{
   int result;
   unsigned long size;

   if (string)
      size = (unsigned long) strlen(string);
   else
      size = 0;

   result = mg_buf_cat_ex(p_mem_obj, string, size);
   p_mem_obj->p_buffer[p_mem_obj->curr_size] = '\0';
   return result;
}


int mg_buf_cat_ex(LPMEMOBJ p_mem_obj, char *buffer, unsigned long size)
{
   int result, stmem;
   unsigned long req_size, tsize, csize, incr_size;
   char *p_temp;
   char buffer_temp[MG_STMEM];

#ifdef _WIN32
__try {
#endif

   result = 1;
   stmem = 0;
   *buffer_temp = '\0';

   if (!buffer)
      return 0;

   req_size = size + p_mem_obj->curr_size;

   if (req_size < MG_STMEM) {
      memcpy((void *) (p_mem_obj->p_buffer + p_mem_obj->curr_size), (void *) buffer, size);
      p_mem_obj->curr_size = req_size;
      p_mem_obj->size = req_size;
   }
   else {
      if (req_size >= p_mem_obj->size) {

         tsize = p_mem_obj->size;
         csize = p_mem_obj->curr_size;
         incr_size = p_mem_obj->incr_size;
         while (req_size >= tsize)
            tsize = tsize + p_mem_obj->incr_size;

         if (p_mem_obj->curr_size < MG_STMEM) {
            memcpy((void *) buffer_temp, (void *) p_mem_obj->p_buffer, csize);
            p_temp = buffer_temp;
            stmem = 1;
         }
         else {
            p_temp = p_mem_obj->p_buffer;
            stmem = 0;
         }
         result = mg_buf_init(p_mem_obj, tsize, incr_size);

         if (result) {
            if (p_temp) {
               memcpy((void *) p_mem_obj->p_buffer, (void *) p_temp, csize);
               if (!stmem) {
                  mg_free((void *) p_temp, "mg_buf_cat:1");
                  p_temp = NULL;
               }
            }
         }
         else {
            if (stmem)
               memcpy((void *) p_mem_obj->p_buffer, (void *) p_temp, csize);
            else
               p_mem_obj->p_buffer = p_temp;
         }
      }
      if (result) {
         memcpy((void *) (p_mem_obj->p_buffer + csize), (void *) buffer, size);
         p_mem_obj->curr_size = req_size;
      }
   }

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_buf_cat_ex: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_mutex_create(MUTOBJ ** lpp_mutex_id, char * mutex_name)
{

   LPMUTOBJ p_mutex_id;

   p_mutex_id = (LPMUTOBJ) mg_malloc(sizeof(MUTOBJ), "mg_mutex_create:1");

   if (p_mutex_id == NULL)
      return -1;

   *lpp_mutex_id = (void *) p_mutex_id;

#if _WIN32

   p_mutex_id->hMutex = CreateMutex(NULL, FALSE, mutex_name);
   return 1;

#else

   pthread_mutex_init(&p_mutex_id->memlock, NULL);
   return 1;

#endif

}


int mg_mutex_lock(MUTOBJ *p_mutex_id)
{

#if _WIN32
   unsigned long dwWaitResult;
#endif

   if (p_mutex_id == NULL)
      return -1;


#if _WIN32

   dwWaitResult = WaitForSingleObject(((LPMUTOBJ) p_mutex_id)->hMutex, 60000L);
   if (dwWaitResult == WAIT_OBJECT_0 || dwWaitResult != 999)
      return 1;
   else {
      return 0;
   }

#else

   pthread_mutex_lock(&((LPMUTOBJ) p_mutex_id)->memlock);
   return 1;

#endif

}


int mg_mutex_unlock(MUTOBJ *p_mutex_id)
{

   int result;

   if (p_mutex_id == NULL)
      return -1;

   result = 0;

#if _WIN32

   if (!ReleaseMutex(((LPMUTOBJ) p_mutex_id)->hMutex)) {
      result = -1;
   }

#else

   pthread_mutex_unlock(&((LPMUTOBJ) p_mutex_id)->memlock);

#endif

   return 1;
}


int mg_mutex_destroy(MUTOBJ *p_mutex_id)
{

   if (p_mutex_id == NULL)
      return -1;

#if _WIN32

#else

   pthread_mutex_destroy(&((LPMUTOBJ) p_mutex_id)->memlock);

#endif

   mg_free((void *) p_mutex_id, "mg_mutex_destroy:1");
   p_mutex_id = NULL;

   return 1;

}


#if 1
int mg_semaphore_create(SEMOBJ ** lpp_sem_obj, char * sem_name)
{
   int result, type;
   LPSEMOBJ p_sem_obj;

#ifdef _WIN32
__try {
#endif

   result = 0;
   type = 1;

   if (*lpp_sem_obj) {
      p_sem_obj = *lpp_sem_obj;
      p_sem_obj->shm_res = 1;
   }
   else {
      p_sem_obj = (LPSEMOBJ) mg_malloc(sizeof(SEMOBJ), "");
      *lpp_sem_obj = p_sem_obj;
      p_sem_obj->shm_res = 0;
   }

   if (p_sem_obj == NULL)
      return -1;

   if (sem_name) {
      strcpy(p_sem_obj->sem_name, sem_name);
   }

#if defined(_WIN32)

   p_sem_obj->hSemaphore = CreateSemaphore(NULL, 1, 32, sem_name);

   if (!p_sem_obj->hSemaphore) {
      DWORD error_code;
      char buffer[1024], errs[256];

      error_code = mg_get_last_error(1);
      mg_get_error_message(error_code, errs, 250, 0);
      T_SPRINTF(buffer, "Semaphore initialization error (CreateSemaphore); name=%s; error no=%d (%s)", sem_name ? sem_name : "null", error_code, errs);
      mg_log_event(buffer, "Error: mg_semaphore_create");
      result = -1;
   }

   result = 1;

#else

   if (type == 1) { /* Memory resident */

      p_sem_obj->semid = sem_init(&(p_sem_obj->memlock), 1, 1);

      if (p_sem_obj->semid) {

         if (sem_name) {
            if (core_data.verbose >= 2) {
               char buffer[1024], errs[256];

               mg_get_error_message(errno, errs, 250, 0);
               T_SPRINTF(buffer, "Memory resident (unnamed) Semaphore initialization error (Will attempt to use POSIX Named Semaphores instead): result=%d; errno=%d (%s)", p_sem_obj->semid, errno, errs);
               mg_log_event(buffer, "Error: mg_semaphore_create (POSIX)");
            }
/*
            if (core_data.p_core_shm) {
               core_data.p_core_shm->sem_global_type = 0;
            }
*/
            result = 0;
            type = 0; /* Try named semaphores instead */
         }

         if (type == 1 && core_data.log_errors) {
            char buffer[1024], errs[256];

            mg_get_error_message(errno, errs, 250, 0);
            T_SPRINTF(buffer, "Memory resident (unnamed) Semaphore initialization error: result=%d; errno=%d (%s)", p_sem_obj->semid, errno, errs);
            mg_log_event(buffer, "Error: mg_semaphore_create (POSIX)");
         }
         result = -2;
      }
      else {

         p_sem_obj->type = 1;
         result = 1;
         goto mg_semaphore_create_exit;
      }
   }

   if (sem_name) {

      p_sem_obj->sd = sem_open(sem_name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 1);

      if (p_sem_obj->sd && p_sem_obj->sd != (sem_t *) SEM_FAILED) {

         /* Success */

         p_sem_obj->type = 0;
         result = 1;
      }
      else if (errno == EEXIST) {

         p_sem_obj->sd = sem_open(sem_name, O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 1);

         if (p_sem_obj->sd && p_sem_obj->sd != (sem_t *) SEM_FAILED) {

            /* Success */

            p_sem_obj->type = 0;
            result = 1;
         }
         else {

            if (core_data.log_errors) {
               char buffer[1024], errs[256];

               mg_get_error_message(errno, errs, 250, 0);
               T_SPRINTF(buffer, "Named Semaphore initialization error (Second Process); name=%s; sd=%p; errno=%d (%s)", sem_name, p_sem_obj->sd, errno, errs);
               mg_log_event(buffer, "Error: mg_semaphore_create (POSIX)");
            }
            result = -2;
         }
      }
      else {
         if (core_data.log_errors) {
            char buffer[1024], errs[256];

            mg_get_error_message(errno, errs, 250, 0);
            T_SPRINTF(buffer, "Named Semaphore initialization error; name=%s; sd=%p; errno=%d (%s)", sem_name, p_sem_obj->sd, errno, errs);
            mg_log_event(buffer, "Error: mg_semaphore_create (POSIX)");
         }

         result = -2;
      }
   }
   else {
      result = 0;
      if (core_data.log_errors) {
         mg_log_event("Name for POSIX Semaphore not available", "Error: mg_semaphore_create (POSIX)");
      }
   }

mg_semaphore_create_exit:

#endif

   p_sem_obj->stack = 0;

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   DWORD code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_semaphore_create: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



int mg_semaphore_lock(SEMOBJ *p_sem_obj)
{
   int result;

#ifdef _WIN32
   DWORD dwWaitResult;
#endif


#ifdef _WIN32
__try {
#endif

   result = 0;

   if (p_sem_obj == NULL)
      return -1;

#if defined(_WIN32)

   dwWaitResult = WaitForSingleObject(p_sem_obj->hSemaphore, INFINITE);

   if (dwWaitResult == WAIT_OBJECT_0)
      result = 1;
   else if (dwWaitResult == WAIT_ABANDONED) {

      char buffer[256];

      T_STRCPY(buffer, "mg_semaphore_lock: Returned WAIT_ABANDONED state");
      mg_log_event(buffer, "Diagnostic");

      result = 1;
   }

   else if (dwWaitResult == WAIT_TIMEOUT) {

      char buffer[256];

      T_STRCPY(buffer, "mg_semaphore_lock: Returned WAIT_TIMEOUT state");
      mg_log_event(buffer, "Diagnostic");

      result = 0;
   }
   else if (dwWaitResult == WAIT_FAILED) {

      char buffer[256];

      T_SPRINTF(buffer, "mg_semaphore_lock: Returned WAIT_FAILED state: Error Code: %d", GetLastError());
      mg_log_event(buffer, "Diagnostic");

      result = 0;
   }
   else {

      char buffer[256];

      T_SPRINTF(buffer, "mg_semaphore_lock: Returned Unrecognized state: %d", dwWaitResult);
      mg_log_event(buffer, "Diagnostic");

      result = 0;
   }


#else

   if (p_sem_obj->type == 0) {
      if (sem_wait(p_sem_obj->sd) == -1)
         result = 0;
      else
         result = 1;
   }
   else if (p_sem_obj->type == 1) {
      if (sem_wait(&(p_sem_obj->memlock)) == -1)
         result = 0;
      else
         result = 1;
   }
   else
      result = 1;

#endif

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   DWORD code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_semaphore_lock: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_semaphore_unlock(SEMOBJ *p_sem_obj)
{
   int result;

#ifdef _WIN32
__try {
#endif

   result = 1;

   if (p_sem_obj == NULL)
      return -1;

#if defined(_WIN32)

   ReleaseSemaphore(p_sem_obj->hSemaphore, 1, NULL);

#else

   if (p_sem_obj->type == 0) {
      if (sem_post(p_sem_obj->sd) == -1)
         result = 0;
      else
         result = 1;
   }
   else if (p_sem_obj->type == 1) {
      if (sem_post(&(p_sem_obj->memlock)) == -1)
         result = 0;
      else
         result = 1;
   }
   else
      result = 1;

#endif

   return result;


#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   DWORD code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_semaphore_unlock: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



int mg_semaphore_destroy(SEMOBJ *p_sem_obj)
{
   int result;

#ifdef _WIN32
__try {
#endif

   if (p_sem_obj == NULL)
      return -1;

   result = 1;

#if defined(_WIN32)

   if (p_sem_obj == NULL)
      return -1;

#else

   if (p_sem_obj && p_sem_obj->type == 0) {
      if (sem_close(p_sem_obj->sd) == -1)
         result = 0;
      else
         result = 1;
   }
   if (p_sem_obj->sem_name) {
      shm_unlink(p_sem_obj->sem_name);
   }
   else if (p_sem_obj && p_sem_obj->type == 1) {
      if (sem_destroy(&(p_sem_obj->memlock)) == -1)
         result = 0;
      else
         result = 1;
   }
   else
      result = 1;

#endif

   if (p_sem_obj && p_sem_obj->shm_res == 0) {
      mg_free((void *) p_sem_obj, 0);
   }

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   DWORD code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_semaphore_destroy: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}
#endif


unsigned long mg_shm_size()
{
   unsigned long shm_size;

   shm_size = sizeof(CORESHM);
   return shm_size;
}


int mg_shm_init(char *shm_name)
{
   short shm;
   int result, retval;
   unsigned long size;
   char buffer[256], shm_name_tmp[256];
#if defined(_WIN32)
   BOOL init = FALSE;
   MGMSSD sd;
#else
   int fd, flags;
   key_t fkey;
#endif

#ifdef _WIN32
__try {
#endif

   result = 0;
   retval = 0;
   *buffer = '\0';
   strcpy(shm_name_tmp, shm_name);

   size = mg_shm_size();

#if defined(_WIN32)

   mg_shm_create_ms_secdesc(&sd);

   core_data.h_core_shm = CreateFileMapping(
                INVALID_HANDLE_VALUE,     /* use paging file (HANDLE) 0xFFFFFFFF : CMT501 */
                sd.pSA,                   /* security attributes */
                PAGE_READWRITE,           /* read/write access */
                0,                        /* size: high 32-bits */
                size,                     /* size: low 32-bits */
                shm_name_tmp);                /* name of map object */

   mg_shm_destroy_ms_secdesc(&sd);

   if (!core_data.h_core_shm) {
      core_data.h_core_shm = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, shm_name_tmp);
   }

   if (core_data.h_core_shm != NULL) {

      /* The first process to attach initializes memory. */
 
      init = (GetLastError() != ERROR_ALREADY_EXISTS); 

      /* Get a pointer to the file-mapped shared memory. */
 
      core_data.p_core_shm = (LPCORESHM) MapViewOfFile( 
               core_data.h_core_shm,     /* object to map view of */
               FILE_MAP_ALL_ACCESS, /* FILE_MAP_WRITE, */ /* read/write access */
               0,              /* high offset:  map from */
               0,              /* low offset:   beginning */
               0);             /* default: map entire file */
      if (core_data.p_core_shm != NULL) {

         shm = 1;

         /* Initialize memory if this is the first process. */

         if (init) {
            memset(core_data.p_core_shm, '\0', sizeof(CORESHM));
            mg_shm_init_data();
         }
      }
      else {

         if (core_data.log_errors) {
            DWORD error_code;
            char buffer[1024], errs[256];

            error_code = mg_get_last_error(1);
            mg_get_error_message(error_code, errs, 250, 0);

            T_SPRINTF(buffer, "Unable to get address of file-mapped shared memory block: Error Code: %d (%s)", error_code, errs);
            mg_log_event(buffer, "Shared Memory Allocation Error");
         }
      }
   }
   else {

      if (core_data.log_errors) {
         DWORD error_code;
         char buffer[1024], errs[256];

         error_code = mg_get_last_error(1);
         mg_get_error_message(error_code, errs, 250, 0);

         T_SPRINTF(buffer, "Unable to create a handle to a shared memory block: Error Code: %d (%s)", error_code, errs);
         mg_log_event(buffer, "Shared Memory Allocation Error");
      }
   }

#else

mg_shm_init_retry:

   flags = O_RDWR | O_CREAT | O_EXCL;

   fd = shm_open(shm_name_tmp, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

   if (fd != -1) {
      retval = ftruncate(fd, size);
      core_data.p_core_shm = (LPCORESHM) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

      if (core_data.p_core_shm != NULL) {

         shm = 1;
         memset(core_data.p_core_shm, '\0', sizeof(CORESHM));
         mg_shm_init_data();
      }
      else {
         if (core_data.log_errors && !T_STRSTR(shm_name_tmp, "/")) {
            char buffer[1024], errs[256];

            mg_get_error_message(errno, errs, 250, 0);
            T_SPRINTF(buffer, "Unable to map to shared memory block (First Process) fd=%d; name=%s; error=%d (%s);", fd, shm_name_tmp, errno, errs);
            mg_log_event(buffer, "Shared Memory Allocation Error");
         }
         result = -1;
      }
   }
   else if (errno == EEXIST) {
      flags = O_RDWR | O_CREAT;
      fd = shm_open(shm_name_tmp, flags, S_IRWXO);
      if (fd != -1) {
         core_data.p_core_shm = (LPCORESHM) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
         if (core_data.p_core_shm != NULL) {
            shm = 1;
         }
         else {
            if (core_data.log_errors && !T_STRSTR(shm_name_tmp, "/")) {
               char buffer[1024], errs[256];

               mg_get_error_message(errno, errs, 250, 0);
               T_SPRINTF(buffer, "Unable to map to shared memory block (Second Process) fd=%d; name=%s; error=%d (%s);", fd, shm_name_tmp, errno, errs);
               mg_log_event(buffer, "Shared Memory Allocation Error");
            }
            result = -1;

         }
      }
      else {
         if (core_data.log_errors && !T_STRSTR(shm_name_tmp, "/")) {
            char buffer[1024], errs[256];

            mg_get_error_message(errno, errs, 250, 0);
            T_SPRINTF(buffer, "Unable to open shared memory block (Second Process) fd=%d; name=%s; error=%d (%s);", fd, shm_name_tmp, errno, errs);
            mg_log_event(buffer, "Shared Memory Allocation Error");
         }
         result = -1;
      }
   }
   else {
      if (core_data.log_errors && !T_STRSTR(shm_name_tmp, "/")) {
         char buffer[1024], errs[256];

         mg_get_error_message(errno, errs, 250, 0);
         T_SPRINTF(buffer, "Unable to create shared memory block (First Process) fd=%d; name=%s; error=%d (%s);", fd, shm_name_tmp, errno, errs);
         mg_log_event(buffer, "Shared Memory Allocation Error");
      }
      result = -1;

      if (strstr(shm_name_tmp + 1, "/")) {
         int n, len, ok;
         char temp[256];

         ok = 0;
         len = T_STRLEN(shm_name_tmp);
         for (n = len - 1; n > 0; n --) {
            if (shm_name_tmp[n] == '/') {
#if defined(SOLARIS)
               T_STRCPY(temp, shm_name_tmp + n);
#else
               T_STRCPY(temp, shm_name_tmp + (n + 1));
#endif
               T_STRCPY(shm_name_tmp, temp);
               ok = 1;
               break;
            }
         }
         if (ok) {
            result = 0;
            goto mg_shm_init_retry;
         }
      }
      else if (shm_name_tmp[0] != '/') {
         char temp[256];

         T_STRCPY(temp, shm_name_tmp);
         shm_name_tmp[0] = '/';
         T_STRCPY(shm_name_tmp + 1, temp);
         result = 0;
         goto mg_shm_init_retry;
      }
   }

#endif

   if (!shm) {
      core_data.p_core_shm = (LPCORESHM) mg_malloc(size, 0);
      memset(core_data.p_core_shm, '\0', sizeof(CORESHM));
      mg_shm_init_data();
   }

   core_data.p_core_shm->shm = shm;
   core_data.p_core_shm->size = size;
   T_STRCPY(core_data.p_core_shm->shm_name, shm_name_tmp);

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   DWORD code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_shm_init: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


#ifdef _WIN32
int mg_shm_create_ms_secdesc(MGMSSD *p_sd)
{
   DWORD dwRes = 0, error_code = 0;
   char buffer[1024], errs[256];
#if 0
   EXPLICIT_ACCESS ea[2];
   SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
   SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
#endif

   p_sd->pSA = NULL;
   p_sd->pEveryoneSID = NULL;
   p_sd->pAdminSID = NULL;
   p_sd->pACL = NULL;
   p_sd->pSD = NULL;

   p_sd->pSA = (PSECURITY_ATTRIBUTES) LocalAlloc(LPTR, sizeof(SECURITY_ATTRIBUTES)); 
   if (p_sd->pSA == NULL) {
      error_code = mg_get_last_error(1);
      mg_get_error_message(error_code, errs, 250, 0);
      T_SPRINTF(buffer, "SECURITY_ATTRIBUTES: LocalAlloc Error: %u (%s)", error_code, errs);
      mg_log_event(buffer, "mg_shm_create_ms_secdesc : Initialization error");
      mg_shm_destroy_ms_secdesc(p_sd);
      return 0;
   }

   p_sd->pSA->nLength = sizeof(SECURITY_ATTRIBUTES);
   p_sd->pSA->bInheritHandle = FALSE;

   p_sd->pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH); 
   if (p_sd->pSD == NULL) {
      error_code = mg_get_last_error(1);
      mg_get_error_message(error_code, errs, 250, 0);
      T_SPRINTF(buffer, "SECURITY_DESCRIPTOR: LocalAlloc Error: %u (%s)", error_code, errs);
      mg_log_event(buffer, "mg_shm_create_ms_secdesc : Initialization error");
      mg_shm_destroy_ms_secdesc(p_sd);
      return 0;
   }

   p_sd->pSA->lpSecurityDescriptor = p_sd->pSD;

   if (!InitializeSecurityDescriptor(p_sd->pSD, SECURITY_DESCRIPTOR_REVISION)) {
      error_code = mg_get_last_error(1);
      mg_get_error_message(error_code, errs, 250, 0);
      T_SPRINTF(buffer, "InitializeSecurityDescriptor Error: %u (%s)", error_code, errs);
      mg_log_event(buffer, "mg_shm_create_ms_secdesc : Initialization error");
      mg_shm_destroy_ms_secdesc(p_sd);
      return 0;
   }

   if (!SetSecurityDescriptorDacl(p_sd->pSD, TRUE, 0, FALSE)) {
      error_code = mg_get_last_error(1);
      mg_get_error_message(error_code, errs, 250, 0);
      T_SPRINTF(buffer, "SetSecurityDescriptorDacl Error: %u (%s)", error_code, errs);
      mg_log_event(buffer, "mg_shm_create_ms_secdesc : Initialization error");
      mg_shm_destroy_ms_secdesc(p_sd);
      return 0;
   } 

   return 1;

#if 0
   /* Create a well-known SID for the Everyone group. */

   if (!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &(p_sd->pEveryoneSID))) {
      error_code = mg_get_last_error(1);
      mg_get_error_message(error_code, errs, 250, 0);
      T_SPRINTF(buffer, "AllocateAndInitializeSid Error: %u (%s)", error_code, errs);
      mg_log_event(buffer, "mg_shm_create_ms_secdesc : Initialization error");
      mg_shm_destroy_ms_secdesc(p_sd);
      return 0;
   }

   /*
      Initialize an EXPLICIT_ACCESS structure for an ACE.
      The ACE will allow Everyone read access to the key.
   */

   ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));
   ea[0].grfAccessPermissions = KEY_READ;
   ea[0].grfAccessMode = SET_ACCESS;
   ea[0].grfInheritance= NO_INHERITANCE;
   ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
   ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
   ea[0].Trustee.ptstrName  = (LPTSTR) p_sd->pEveryoneSID;

   /* Create a SID for the BUILTIN\Administrators group. */

   if (!AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &(p_sd->pAdminSID))) {
      error_code = mg_get_last_error(1);
      mg_get_error_message(error_code, errs, 250, 0);
      T_SPRINTF(buffer, "AllocateAndInitializeSid Error: %u (%s)", error_code, errs);
      mg_log_event(buffer, "mg_shm_create_ms_secdesc : Initialization error");
      mg_shm_destroy_ms_secdesc(p_sd);
      return 0;
   }

   /*
      Initialize an EXPLICIT_ACCESS structure for an ACE.
      The ACE will allow the Administrators group full access to
      the key.
   */

   ea[1].grfAccessPermissions = KEY_ALL_ACCESS;
   ea[1].grfAccessMode = SET_ACCESS;
   ea[1].grfInheritance= NO_INHERITANCE;
   ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
   ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
   ea[1].Trustee.ptstrName  = (LPTSTR) p_sd->pAdminSID;

   /* Create a new ACL that contains the new ACEs. */

   dwRes = SetEntriesInAcl(2, ea, NULL, &(p_sd->pACL));
   if (ERROR_SUCCESS != dwRes) {
      error_code = mg_get_last_error(1);
      mg_get_error_message(error_code, errs, 250, 0);
      T_SPRINTF(buffer, "SetEntriesInAcl Error: %u (%s)", error_code, errs);
      mg_log_event(buffer, "mg_shm_create_ms_secdesc : Initialization error");
      mg_shm_destroy_ms_secdesc(p_sd);
      return 0;
   }

   /* Initialize a security descriptor. */

   p_sd->pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(SECURITY_DESCRIPTOR_MIN_LENGTH); 
   if (p_sd->pSD == NULL) {
      error_code = mg_get_last_error(1);
      mg_get_error_message(error_code, errs, 250, 0);
      T_SPRINTF(buffer, "SECURITY_DESCRIPTOR: LocalAlloc Error: %u (%s)", error_code, errs);
      mg_log_event(buffer, "mg_shm_create_ms_secdesc : Initialization error");
      mg_shm_destroy_ms_secdesc(p_sd);
      return 0;
   }
 
   if (!InitializeSecurityDescriptor(p_sd->pSD, SECURITY_DESCRIPTOR_REVISION)) {
      error_code = mg_get_last_error(1);
      mg_get_error_message(error_code, errs, 250, 0);
      T_SPRINTF(buffer, "InitializeSecurityDescriptor Error: %u (%s)", error_code, errs);
      mg_log_event(buffer, "mg_shm_create_ms_secdesc : Initialization error");
      mg_shm_destroy_ms_secdesc(p_sd);
      return 0;
   }
 
   /* Add the ACL to the security descriptor. bDaclPresent flag/not a default DACL */

   if (!SetSecurityDescriptorDacl(p_sd->pSD, TRUE, p_sd->pACL, FALSE)) {
      error_code = mg_get_last_error(1);
      mg_get_error_message(error_code, errs, 250, 0);
      T_SPRINTF(buffer, "SetSecurityDescriptorDacl Error: %u (%s)", error_code, errs);
      mg_log_event(buffer, "mg_shm_create_ms_secdesc : Initialization error");
      mg_shm_destroy_ms_secdesc(p_sd);
      return 0;
   } 

    /* Initialize a security attributes structure. */

    p_sd->pSA->nLength = sizeof(SECURITY_ATTRIBUTES);
    p_sd->pSA->lpSecurityDescriptor = p_sd->pSD;
    p_sd->pSA->bInheritHandle = FALSE;

    return 1;

#endif

}


int mg_shm_destroy_ms_secdesc(MGMSSD *p_sd)
{

   if (p_sd->pEveryoneSID) 
      FreeSid(p_sd->pEveryoneSID);
   if (p_sd->pAdminSID) 
      FreeSid(p_sd->pAdminSID);
   if (p_sd->pACL)
      LocalFree(p_sd->pACL);
   if (p_sd->pSD)
      LocalFree(p_sd->pSD);
   if (p_sd->pSA)
      LocalFree(p_sd->pSA);

   p_sd->pSA = NULL;
   p_sd->pEveryoneSID = NULL;
   p_sd->pAdminSID = NULL;
   p_sd->pACL = NULL;
   p_sd->pSD = NULL;

   return 1;
}
#endif /* #ifdef _WIN32 */


int mg_shm_init_data(void)
{
#ifdef _WIN32
__try {
#endif

   if (!core_data.p_core_shm)
      return 0;

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   DWORD code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_shm_init_data: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_shm_destroy(short context)
{
   short phase;
   int fd, flags;
   char shm_name_tmp[256];
#ifdef _WIN32
   BOOL ignore = FALSE;
#endif

   phase = 0;

#ifdef _WIN32
__try {
#endif

   if (core_data.p_core_shm && !core_data.p_core_shm->shm) {
      phase = 1;
      mg_free((void *) core_data.p_core_shm, 0);
      goto mg_shm_destroy_exit;
   }

   fd = 0;
   flags = 0;
   strcpy(shm_name_tmp,  core_data.p_core_shm->shm_name);

#if defined(_WIN32)

   if (core_data.p_core_shm && core_data.p_core_shm->shm) {
      phase = 2;

      /* Unmap shared memory from the process's address space. */
 
      ignore = UnmapViewOfFile(core_data.p_core_shm);
 
      /* Close the process's handle to the file-mapping object. */
 
      phase = 3;

      ignore = CloseHandle(core_data.h_core_shm);

      phase = 4;
   }

#else

   if (context >= 1) {
      unlink(shm_name_tmp);
      shm_unlink(shm_name_tmp);
   }

#endif

   phase = 5;

mg_shm_destroy_exit:

   core_data.p_core_shm = NULL;

   phase = 6;

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   DWORD code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_shm_destroy: %d|%x", phase, code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



int mg_thread_create(LPTHRCTRL p_thread_control, MG_THR_START_ROUTINE start_routine, void *arg)
{
#ifdef _WIN32

   int n, result;
   unsigned long CreationFlags, StackSize;
   LPSECURITY_ATTRIBUTES lpThreadAttributes;

   n = 0;
   result = 0;
   StackSize = 0;

   lpThreadAttributes = NULL;
   CreationFlags = 0;

   p_thread_control->hThread = CreateThread(lpThreadAttributes,
            /* pointer to thread security attributes */
            StackSize,
            /* initial thread stack size, in bytes */
            (MG_THR_START_ROUTINE) start_routine,
            /* pointer to thread function */
            (void *) arg,
            /* argument for new thread */
            CreationFlags,
            /* creation flags */ 
            (LPDWORD) &(p_thread_control->thread_id)
            /* pointer to returned thread identifier */
            );

   if (p_thread_control->hThread) {
      result = 1;

      n = mg_mutex_lock((MUTOBJ *) core_data.p_memlockTH);

      for (n = 0; n < MG_MAX_CONNECTIONS; n ++) {
         if (!core_data.thread_handles[n]) {
            core_data.thread_handles[n] = p_thread_control->hThread;
            break;
         }
      }

      mg_mutex_unlock((MUTOBJ *) core_data.p_memlockTH);

   }
   else
      result = -1;


#else

   int result;
   size_t StackSize;
   pthread_attr_t attr;

   StackSize = 0;

   pthread_attr_init(&attr);

#if defined(OSF1) || defined(HPUX) || defined(HPUX10) || defined(HPUX11) || defined(FREEBSD) || defined(AIX) || defined(AIX5)
   pthread_attr_getstacksize(&attr, &StackSize);

#if defined(AIX) || defined(AIX5)
   pthread_attr_setstacksize(&attr, 2000000); /* StackSize * 6); */
#elif defined(OSF1)
   pthread_attr_setstacksize(&attr, 1000000); /* StackSize * 4); */
#elif defined(LINUX)
   pthread_attr_setstacksize(&attr, 2000000); /* StackSize * 4); */
#else
   pthread_attr_setstacksize(&attr, StackSize * 4);
#endif

   result = pthread_create(&(p_thread_control->thread_id),
                           &attr,
                           (MG_THR_START_ROUTINE) start_routine,
                           (void *) arg);
#else

   result = pthread_create(&(p_thread_control->thread_id),
                           NULL,
                           (MG_THR_START_ROUTINE) start_routine,
                           (void *) arg);


#endif

#endif

   return result;

}



int mg_thread_detach(void)
{
   int result;

   result = 1;

#if !defined(_WIN32)

   result = pthread_detach(pthread_self());

#endif

   return result;
}


int mg_thread_exit(void)
{

#ifdef _WIN32

   ExitThread(0);

#else

   pthread_exit(NULL);

#endif

   return 1;
}


unsigned long mg_current_thread_id(void)
{

#if defined(_WIN32)

   return (unsigned long) GetCurrentThreadId();

#else

   return ((unsigned long) pthread_self());

#endif

}


unsigned long mg_current_process_id(void)
{
#if defined(_WIN32)

   return (unsigned long) GetCurrentProcessId();

#else

   return ((unsigned long) getpid());

#endif
}


int mg_insert_error_size(char * error)
{
   int size, hlen;
   unsigned char esize[32];

   size = (int) strlen(error) - MG_RECV_HEAD;
   if (size < 0)
      return 0;

   hlen = mg_encode_size(esize, size, MG_CHUNK_SIZE_BASE);
   strncpy(error + (5 - hlen), esize, hlen);
   return 1;
}


int mg_pow(int n10, int power)
{
#ifdef _WIN32
   return (int) pow((double) n10, (double) power);
#else
   int n, result;
   if (power == 0)
      return 1;
   result = 1;
   for (n = 1; n <= power; n ++)
      result = result * n10;
   return result;
#endif
}


int mg_encode_size64(int n10)
{
   if (n10 >= 0 && n10 < 10)
      return (48 + n10);
   if (n10 >= 10 && n10 < 36)
      return (65 + (n10 - 10));
   if (n10 >= 36 && n10 < 62)
      return  (97 + (n10 - 36));

   return 0;
}


int mg_decode_size64(int nxx)
{
   if (nxx >= 48 && nxx < 58)
      return (nxx - 48);
   if (nxx >= 65 && nxx < 91)
      return ((nxx - 65) + 10);
   if (nxx >= 97 && nxx < 123)
      return ((nxx - 97) + 36);

   return 0;
}


int mg_encode_size(unsigned char *esize, int size, short base)
{
   if (base == 10) {
      sprintf((char *) esize, "%d", size);
      return (int) strlen((char *) esize);
   }
   else {
      int n, n1, x;
      char buffer[32];

      n1 = 31;
      buffer[n1 --] = '\0';
      buffer[n1 --] = mg_encode_size64(size  % base);

      for (n = 1;; n ++) {
         x = (size / mg_pow(base, n));
         if (!x)
            break;
         buffer[n1 --] = mg_encode_size64(x  % base);
      }
      n1 ++;
      strcpy((char *) esize, buffer + n1);
      return (int) strlen((char *) esize);
   }
}


int mg_decode_size(unsigned char *esize, int len, short base)
{
   int size;
   unsigned char c;

   if (base == 10) {
      c = *(esize + len);
      *(esize + len) = '\0';
      size = (int) strtol(esize, NULL, 10);
      *(esize + len) = c;
   }
   else {
      int n, x;

      size = 0;
      for (n = len - 1; n >= 0; n --) {
         x = (int) esize[n];
         size = size + mg_decode_size64(x) * mg_pow(base, (len - (n + 1)));
      }
   }

   return size;
}


int mg_encode_item_header(unsigned char * head, int size, short byref, short type)
{
   int slen, hlen;
   unsigned int code;
   unsigned char esize[16];

   slen = mg_encode_size(esize, size, 10);

   code = slen + (type * 8) + (byref * 64);
   head[0] = (unsigned char) code;
   strncpy(head + 1, esize, slen);

   hlen = slen + 1;
   head[hlen] = '0';

   return hlen;
}


int mg_decode_item_header(unsigned char * head, int * size, short * byref, short * type)
{
   int slen, hlen;
   unsigned int code;

   code = (unsigned int) head[0];

   *byref = code / 64;
   *type = (code % 64) / 8;
   slen = code % 8;

   *size = mg_decode_size(head + 1, slen, 10);

   hlen = slen + 1;

   return hlen;
}


int mg_make_token(char *token, int length, int type)
{
   int n, len;
   char buffer[64], temp[64];

#ifdef _WIN32
__try {
#endif

   *token = '\0';
   len = 0;

   T_STRCPY(temp, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

   if (type == 0) {
      core_data.session_no ++;
      if (core_data.session_no > 1000000)
         core_data.session_no = 1;
      T_SPRINTF(buffer, "%ld", core_data.session_no);
      len = (int) T_STRLEN(buffer);
      T_SPRINTF(token, "%d", len);
      T_STRCAT(token, buffer);
      len = (int) T_STRLEN(token);
   }

   srand((unsigned) time(NULL));

   if (type == 2) {
      for (;;) {
         n = rand() % 62;
         token[len ++] = temp[n];
         if (len == length) {
            token[len] = '\0';
            break;
         }
      }
   }
   else {
      for (;;) {

         n = rand();
         T_SPRINTF(buffer, "%d", n);

         for (n = 0; buffer[n]; n ++) {
            token[len ++] = buffer[n];
            if (len == length)
               break;
         }
         if (len == length) {
            token[len] = '\0';
            break;
         }
      }
   }

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_make_token: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


unsigned long mg_crc32(char *buffer, size_t len)
{
   register unsigned long oldcrc32;

   oldcrc32 = 0xFFFFFFFF;

   for ( ; len; --len, ++buffer) {
      oldcrc32 = UPDC32(*buffer, oldcrc32);
   }

   return ~oldcrc32;      
}



int mg_date_components(char *date, int *dd, int *mm, int *yyyy, char *month)
{
   int d[4], n, n1, result;

   result = 0;
   *dd = 0;
   *mm = 0;
   *yyyy = 0;

   if (month)
      *month = '\0';

   for (n = 0, n1 = 0; date[n]; n ++) {
      if (!n)
         d[n1 ++] = (int) strtol(date + n, NULL, 10);
      else if (isdigit(date[n]) && !isdigit(date[n - 1]))
         d[n1 ++] = (int) strtol(date + n, NULL, 10);
      if (n1 == 3)
         break;
   }

   if (n1 == 3) {
      if (d[2] < 2000)
         d[2] += 2000;

      if (d[0] > 0 && d[0] < 32 && d[1] > 0 && d[1] < 13) {
         *dd = d[0];
         *mm = d[1];
         *yyyy = d[2];
         result = 1;
         if (month)
            strcpy(month, month_list[d[1]]);
      }

   }

   return result;

}


int mg_date_encode(int dd, int mm, int yyyy)
{
   struct tm ds = {0};
   time_t t;

   ds.tm_year = (yyyy - 2000) + 100;
   ds.tm_mon = mm - 1;
   ds.tm_mday = dd;

   t = mktime(&ds);

   if (t == (time_t) -1)
      return 0;

   return mg_date_days((time_t) t);
}


int mg_date_days(time_t d)
{
   int result;

   result = ((int) d) / 86400;

   return result;
}


double mg_get_time(char * timestr)
{
   int tv_msec;
   double tms;
   time_t t_now, t;
   char *timeline, *p;
   char buffer[32];

#if !defined(_WIN32)

   int n;
   struct timeval tv; 

   n = gettimeofday(&tv, 0);

   t = tv.tv_sec % 86400; 
   tms = t + ((double) tv.tv_usec / 1000000);
   t_now = tv.tv_sec;
   tv_msec = tv.tv_usec / 1000;

#else

#ifdef _WIN32
   struct _timeb timebuffer;
#else
   struct timeb timebuffer;
#endif

/*
   tms = (double) clock() / CLOCKS_PER_SEC;
*/

#ifdef _WIN32
   _ftime(&timebuffer);
#else
   ftime(&timebuffer);
#endif

   t = timebuffer.time % 86400; 
   tms = t + ((double) timebuffer.millitm / 1000);
   t_now = timebuffer.time;
   tv_msec = timebuffer.millitm;

#endif

   if (timestr) {
      timeline = ctime(&t_now);
      p = strstr(timeline, ":");
      if (p) {
         p -= 2;
         strncpy(buffer, p, 8);
         buffer[8] = '\0';
         T_SPRINTF(timestr, "%s.%03d", buffer, tv_msec);
      }
   }

   return tms;

}


int mg_piece(char *outstring, char *instring, char *delimiter, int from, int to)
{

   int n, dlen;
   char *p1, *p2, *wk1, *wk2;

#ifdef _WIN32
__try {
#endif

   p1 = NULL;
   p2 = NULL;
   dlen = (int) strlen(delimiter);

   if (!dlen || from < 1 || to < from)
      return 0;
 
   wk1 = instring;

   for (n = 1; ;n ++) {

      if (n == from) {
         if (n == 1)
            p1 = instring;
         else
            p1 = wk2 += dlen;
      }

      wk2 = strstr(wk1, delimiter);

      if (!wk2)
         break;

      if (n == to) {
         p2 = wk2;
         break;
      }

      wk1 = wk2 + 1;
   }

   if (p1) {
      if (!p2)
         strcpy(outstring, p1);
      else {
         for (n = 0; *p1 ;n ++, p1 ++) {
            outstring[n] = *p1;
            if (p1 == p2)
               break;
         }
         outstring[n] = '\0';
      }
   }
   else
      *outstring = '\0';


   return (int) T_STRLEN(outstring);



#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_piece: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_ucase(char *string)
{
   int n, chr;

   n = 0;
   while (string[n] != '\0') {
      chr = (int) string[n];
      if (chr >= 97 && chr <= 122)
         string[n] = (char) (chr - 32);
      n ++;
   }
   return 1;
}


int mg_lcase(char *string)
{
   int n, chr;

   n = 0;
   while (string[n] != '\0') {
      chr = (int) string[n];
      if (chr >= 65 && chr <= 90)
         string[n] = (char) (chr + 32);
      n ++;
   }
   return 1;
}

int mg_trim_string(char *string)
{
   int n, n1, char1, charz;
   char xchar;
   unsigned int nchar;

   char1 = 0, charz = -1;
   n = 0, n1 = 0;
   while (string[n] != '\0') {
      xchar = (char) string[n];
      nchar = (unsigned int) string[n];
      n ++;
      if (char1 == 0 && xchar == ' ')
         continue;
      if (nchar < 32) {
         string[n1] = '\0';
         break;
      }
      char1 = 1;
      if (xchar != ' ')
         charz = n1;
      string[n1] = xchar;
      n1 ++;
   }
   charz ++;
   string[charz] = '\0';

   return charz;
}


int mg_pause(unsigned long msecs)
{

#if !defined(_WIN32)

   int secs;
   secs = msecs / 1000;
   if (secs == 0)
      secs = 1;
   sleep(secs);
   return 1;


#else

   Sleep(msecs);

#endif

   return 1;
}


char mg_b64_ntc(unsigned char n)
{
   if (n < 26)
      return 'A' + n;
   if (n < 52)
      return 'a' - 26 + n;

   if (n < 62)
      return '0' - 52 + n;
   if (n == 62)
      return '+';

   return '/';
}


unsigned char mg_b64_ctn(char c)
{

   if (c == '/')
      return 63;
   if (c == '+')
      return 62;
   if ((c >= 'A') && (c <= 'Z'))
      return c - 'A';
   if ((c >= 'a') && (c <= 'z'))
      return c - 'a' + 26;
   if ((c >= '0') && (c <= '9'))
      return c - '0' + 52;
   if (c == '=')
      return 80;
   return 100;
}


int mg_b64_encode(char *from, char *to, int length, int quads)
{
/*
   3 8bit numbers become four characters
*/

   int i = 0;
   char *tot = to;
   int qc = 0; /* Quadcount */
   unsigned char c;
   unsigned char d;

   while (i < length) {
      c = from[i];
      *to++ = (char) mg_b64_ntc((unsigned char) (c / 4));
      c = c * 64;
     
      i++;

      if (i >= length) {
         *to++ = mg_b64_ntc((unsigned char) (c / 4));
         *to++ = '=';
         *to++ = '=';
         break;
      }
      d = from[i];
      *to++ = mg_b64_ntc((unsigned char) (c / 4 + d / 16));
      d = d * 16;

      i++;


      if (i >= length) {
         *to++ = mg_b64_ntc((unsigned char) (d / 4));
         *to++ = '=';
         break;
      }
      c = from[i];
      *to++ = mg_b64_ntc((unsigned char) (d / 4 + c / 64));
      c=c * 4;

      i++;

      *to++ = mg_b64_ntc((unsigned char) (c / 4));

      qc ++; /* qz will never be zero, quads = 0 means no linebreaks */
      if (qc == quads) {
         *to++ = '\n';
         qc = 0;
      }
   }

/*
   if ((quads != 0) && (qc != 0))
      *to ++ = '\n';
*/

/* Insert last linebreak */

   return ((int) (to - tot));
}


int mg_b64_decode(char *from, char *to, int length)
{
   unsigned char c, d, e, f;
   char A, B, C;
   int i;
   int add;
   char *tot = to;

   for (i = 0; i + 3 < length;) {
      add = 0;
      A = B = C = 0;
      c = d = e = f = 100;

      while ((c == 100) && (i < length))
         c = mg_b64_ctn(from[i++]);
      while ((d == 100) && (i < length))
         d = mg_b64_ctn(from[i++]);
      while ((e == 100) && (i < length))
         e = mg_b64_ctn(from[i++]);
      while ((f == 100) && (i < length))
         f = mg_b64_ctn(from[i++]);

      if (f == 100)
         return -1; /* Not valid end */

      if (c < 64) {
         A += c * 4;
         if (d < 64) {
            A += d / 16;

            B += d * 16;

            if (e < 64) {
               B += e / 4;
               C += e * 64;

               if (f < 64) {
                  C += f;
                  to[2] = C;
                  add += 1;

               }
               to[1] = B;
               add += 1;

            }
            to[0] = A;
            add += 1;
         }
      }
      to += add;

      if (f == 80)
         return ((int) (to - tot)); /* end because '=' encountered */
   }
   return ((int) (to - tot));
}


int mg_b64_encoded_buf_size(int l, int q)
{
   int ret;

   ret = (l / 3) * 4;
   if (l % 3 != 0)
      ret += 4;
   if (q != 0) {
      ret += (ret / (q * 4));
   }
   return ret;
}


int mg_b64_strip_encoded_buf(char *buf, int length)
{
   int i;
   int ret = 0;

   for (i = 0;i < length;i ++)
      if (mg_b64_ctn(buf[i]) != 100)
         buf[ret++] = buf[i];
 
   return ret;

}


int mg_log_event(char *buffer, char *title)
{
   int len, n, mem_alloc;
   HANDLE hLogfile = 0;
   unsigned long dwPos = 0, dwBytesWritten = 0;
   FILE *fp = NULL;
   char *pbuffer;
   char timestr[128], s_buffer[1024];
   time_t now = 0;

#ifdef _WIN32
__try {
#endif

   *s_buffer = '\0';
   mem_alloc = 0;

   now = time(NULL);
   T_SPRINTF(timestr, ">>> %s", T_CTIME(&now));

   for (n = 0; timestr[n] != '\0'; n ++) {
      if ((unsigned int) timestr[n] < 32) {
         timestr[n] = '\0';
         break;
      }
   }

   T_SPRINTF(s_buffer, "%s; Version: %s; Thread ID: %lu", timestr, MG_VERSION, mg_current_thread_id());
   T_STRCPY(timestr, s_buffer);


   len = (int) T_STRLEN(timestr) + (int) T_STRLEN(title) + (int) T_STRLEN(buffer) + 32;

   if (len > 1000) {

      pbuffer = (char *) mg_malloc(sizeof(char) * len, "mg_log_event:1");

      if (pbuffer == NULL)
         return 0;

      mem_alloc = 1;
   }
   else {
      pbuffer = s_buffer;
   }

   pbuffer[0] = '\0';
   T_STRCPY(pbuffer, timestr);
   T_STRCAT(pbuffer, "\r\n    ");
   T_STRCAT(pbuffer, title);
   T_STRCAT(pbuffer, "\r\n    ");

   len = (int) strlen(pbuffer);

   pbuffer[len] = '\0';

   strcat(pbuffer, buffer);

   len = (int) T_STRLEN(pbuffer) * sizeof(char);

#if _WIN32

   T_STRCAT(pbuffer, "\r\n");
   len = len + (2 * sizeof(char));
   hLogfile = CreateFile(core_data.mg_log, GENERIC_WRITE, FILE_SHARE_WRITE,
                         (LPSECURITY_ATTRIBUTES) NULL, OPEN_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
   dwPos = SetFilePointer(hLogfile, 0, (LPLONG) NULL, FILE_END);
   LockFile(hLogfile, dwPos, 0, dwPos + len, 0);
   WriteFile(hLogfile, (LPTSTR) pbuffer, len, &dwBytesWritten, NULL);
   UnlockFile(hLogfile, dwPos, 0, dwPos + len, 0);
   CloseHandle(hLogfile);

#else

   T_STRCAT(pbuffer, "\n");

   n = mg_mutex_lock((MUTOBJ *) core_data.p_memlockLG);

   fp = fopen(core_data.mg_log, "a");
   if (fp) {
      fputs(pbuffer, fp);
      fclose(fp);
   }

   n = mg_mutex_unlock((MUTOBJ *) core_data.p_memlockLG);

#endif

   if (mem_alloc) {
      mg_free((void *) pbuffer, "mg_log_event:1");
      pbuffer = NULL;
   }

   return 1;


#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {
   return 0;
}
#endif

}


int mg_log_buffer(unsigned char * buffer, unsigned long buffer_size, char *title)
{
   unsigned int c, len, strt;
   unsigned long n, n1, nc, size;
   char tmp[16];
   char *p;

#ifdef _WIN32
__try {
#endif

   for (n = 0, nc = 0; n < buffer_size; n ++) {
      c = (unsigned int) buffer[n];
      if (c < 32 || c > 126)
         nc ++;
   }

   size = buffer_size + (nc * 4) + 32;
   p = (char *) mg_malloc(sizeof(char) * size, "mg_log_buffer:1");
   if (!p)
      return 0;

   if (nc) {

      for (n = 0, nc = 0; n < buffer_size; n ++) {
         c = (unsigned int) buffer[n];
         if (c < 32 || c > 126) {
            sprintf((char *) tmp, "%02x", c);
            len = (unsigned int) T_STRLEN(tmp);
            if (len > 2)
               strt = len - 2;
            else
               strt = 0;
            p[nc ++] = '\\';
            p[nc ++] = 'x';
            for (n1 = strt; tmp[n1]; n1 ++)
               p[nc ++] = tmp[n1];
         }
         else
            p[nc ++] = buffer[n];
      }
      p[nc] = '\0';
   }
   else {
      strncpy(p, buffer, buffer_size);
      p[buffer_size] = '\0';
   }

   mg_log_event((char *) p, title);

   mg_free((void *) p, "mg_log_buffer:1");

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_log_buffer: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


unsigned long mg_get_last_error(int context)
{
   unsigned long error_code;

#if !defined(_WIN32)
   error_code = (unsigned long) errno;
#else /* Windows */

   if (context)
      error_code = GetLastError();
   else 
      error_code = MGNET_WSAGETLASTERROR();
#endif
   return error_code;
}


int mg_get_error_message(unsigned long error_code, char *message, int size, int context)
{

#if defined(_WIN32)

   if (context == 0) {
      short ok;
      int len;
      char *p;
      void *lpMsgBuf;

      ok = 0;
      lpMsgBuf = NULL;
      len = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                           NULL,
                           error_code,
                           /* MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), */
                           MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                           (LPTSTR) &lpMsgBuf,
                           0,
                           NULL 
                           );
      if (len && lpMsgBuf) {
         strncpy(message, lpMsgBuf, size);
         p = strstr(message, "\r\n");
         if (p)
            *p = '\0';
         ok = 1;
      }
      if (lpMsgBuf)
         LocalFree(lpMsgBuf);

      if (!ok) {
         switch (error_code) {
            case EXCEPTION_ACCESS_VIOLATION:
               strncpy(message, "The thread attempted to read from or write to a virtual address for which it does not have the appropriate access.", size);
               break;
            case EXCEPTION_BREAKPOINT:
               strncpy(message, "A breakpoint was encountered.", size); 
               break;
            case EXCEPTION_DATATYPE_MISALIGNMENT:
               strncpy(message, "The thread attempted to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries, 32-bit values on 4-byte boundaries, and so on.", size);
               break;
            case EXCEPTION_SINGLE_STEP:
               strncpy(message, "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.", size);
               break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
               strncpy(message, "The thread attempted to access an array element that is out of bounds, and the underlying hardware supports bounds checking.", size);
               break;
            case EXCEPTION_FLT_DENORMAL_OPERAND:
               strncpy(message, "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.", size);
               break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
               strncpy(message, "The thread attempted to divide a floating-point value by a floating-point divisor of zero.", size);
               break;
            case EXCEPTION_FLT_INEXACT_RESULT:
               strncpy(message, "The result of a floating-point operation cannot be represented exactly as a decimal fraction.", size);
               break;
            case EXCEPTION_FLT_INVALID_OPERATION:
               strncpy(message, "This exception represents any floating-point exception not included in this list.", size);
               break;
            case EXCEPTION_FLT_OVERFLOW:
               strncpy(message, "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.", size);
               break;
            case EXCEPTION_FLT_STACK_CHECK:
               strncpy(message, "The stack overflowed or underflowed as the result of a floating-point operation.", size);
               break;
            case EXCEPTION_FLT_UNDERFLOW:
               strncpy(message, "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.", size);
               break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
               strncpy(message, "The thread attempted to divide an integer value by an integer divisor of zero.", size);
               break;
            case EXCEPTION_INT_OVERFLOW:
               strncpy(message, "The result of an integer operation caused a carry out of the most significant bit of the result.", size);
               break;
            case EXCEPTION_PRIV_INSTRUCTION:
               strncpy(message, "The thread attempted to execute an instruction whose operation is not allowed in the current machine mode.", size);
               break;
            case EXCEPTION_NONCONTINUABLE_EXCEPTION:
               strncpy(message, "The thread attempted to continue execution after a noncontinuable exception occurred.", size);
               break;
            default:
               strncpy(message, "Unrecognised system or hardware error.", size);
            break;
         }
      }
   }

#else
   if (context == 0) {
      strcpy(message, "");
#if defined(LINUX) || defined(AIX) || defined(OSF1) || defined(MACOSX)
      strerror_r(error_code, message, (size_t) size);
#else
      mg_get_std_error_message(error_code, message, size, context);
#endif
   }

#endif

   message[size - 1] = '\0';

   return (int) strlen(message);
}


int mg_get_std_error_message(unsigned long error_code, char *message, int size, int context)
{

   strcpy(message, "");

#if !defined(_WIN32)
   switch (error_code) {
      case E2BIG:
         strncpy(message, "Argument list too long.", size);
         break;
      case EACCES:
         strncpy(message, "Permission denied.", size);
         break;
      case EADDRINUSE:
         strncpy(message, "Address in use.", size);
         break;
      case EADDRNOTAVAIL:
         strncpy(message, "Address not available.", size);
         break;
      case EAFNOSUPPORT:
         strncpy(message, "Address family not supported.", size);
         break;
      case EAGAIN:
         strncpy(message, "Resource unavailable, try again.", size);
         break;
      case EALREADY:
         strncpy(message, "Connection already in progress.", size);
         break;
      case EBADF:
         strncpy(message, "Bad file descriptor.", size);
         break;
#if !defined(MACOSX) && !defined(FREEBSD)
      case EBADMSG:
         strncpy(message, "Bad message.", size);
         break;
#endif
      case EBUSY:
         strncpy(message, "Device or resource busy.", size);
         break;
      case ECANCELED:
         strncpy(message, "Operation canceled.", size);
         break;
      case ECHILD:
         strncpy(message, "No child processes.", size);
         break;
      case ECONNABORTED:
         strncpy(message, "Connection aborted.", size);
         break;
      case ECONNREFUSED:
         strncpy(message, "Connection refused.", size);
         break;
      case ECONNRESET:
         strncpy(message, "Connection reset.", size);
         break;
      case EDEADLK:
         strncpy(message, "Resource deadlock would occur.", size);
         break;
      case EDESTADDRREQ:
         strncpy(message, "Destination address required.", size);
         break;
      case EDOM:
         strncpy(message, "Mathematics argument out of domain of function.", size);
         break;
      case EDQUOT:
         strncpy(message, "Reserved.", size);
         break;
      case EEXIST:
         strncpy(message, "File exists.", size);
         break;
      case EFAULT:
         strncpy(message, "Bad address.", size);
         break;
      case EFBIG:
         strncpy(message, "File too large.", size);
         break;
      case EHOSTUNREACH:
         strncpy(message, "Host is unreachable.", size);
         break;
      case EIDRM:
         strncpy(message, "Identifier removed.", size);
         break;
      case EILSEQ:
         strncpy(message, "Illegal byte sequence.", size);
         break;
      case EINPROGRESS:
         strncpy(message, "Operation in progress.", size);
         break;
      case EINTR:
         strncpy(message, "Interrupted function.", size);
         break;
      case EINVAL:
         strncpy(message, "Invalid argument.", size);
         break;
      case EIO:
         strncpy(message, "I/O error.", size);
         break;
      case EISCONN:
         strncpy(message, "Socket is connected.", size);
         break;
      case EISDIR:
         strncpy(message, "Is a directory.", size);
         break;
      case ELOOP:
         strncpy(message, "Too many levels of symbolic links.", size);
         break;
      case EMFILE:
         strncpy(message, "Too many open files.", size);
         break;
      case EMLINK:
         strncpy(message, "Too many links.", size);
         break;
      case EMSGSIZE:
         strncpy(message, "Message too large.", size);
         break;
#if !defined(MG_VMS) && !defined(MACOSX) && !defined(OSF1) && !defined(FREEBSD)
      case EMULTIHOP:
         strncpy(message, "Reserved.", size);
         break;
#endif
      case ENAMETOOLONG:
         strncpy(message, "Filename too long.", size);
         break;
      case ENETDOWN:
         strncpy(message, "Network is down.", size);
         break;
      case ENETRESET:
         strncpy(message, "Connection aborted by network.", size);
         break;
      case ENETUNREACH:
         strncpy(message, "Network unreachable.", size);
         break;
      case ENFILE:
         strncpy(message, "Too many files open in system.", size);
         break;
      case ENOBUFS:
         strncpy(message, "No buffer space available.", size);
         break;
#if !defined(MG_VMS) && !defined(MACOSX) && !defined(FREEBSD)
      case ENODATA:
         strncpy(message, "[XSR] [Option Start] No message is available on the STREAM head read queue. [Option End]", size);
         break;
#endif
      case ENODEV:
         strncpy(message, "No such device.", size);
         break;
      case ENOENT:
         strncpy(message, "No such file or directory.", size);
         break;
      case ENOEXEC:
         strncpy(message, "Executable file format error.", size);
         break;
      case ENOLCK:
         strncpy(message, "No locks available.", size);
         break;
#if !defined(MG_VMS) && !defined(MACOSX) && !defined(OSF1) && !defined(FREEBSD)
      case ENOLINK:
         strncpy(message, "Reserved.", size);
         break;
#endif
      case ENOMEM:
         strncpy(message, "Not enough space.", size);
         break;
      case ENOMSG:
         strncpy(message, "No message of the desired type.", size);
         break;
      case ENOPROTOOPT:
         strncpy(message, "Protocol not available.", size);
         break;
      case ENOSPC:
         strncpy(message, "No space left on device.", size);
         break;
#if !defined(MG_VMS) && !defined(MACOSX) && !defined(FREEBSD)
      case ENOSR:
         strncpy(message, "[XSR] [Option Start] No STREAM resources. [Option End]", size);
         break;
#endif
#if !defined(MG_VMS) && !defined(MACOSX) && !defined(FREEBSD)
      case ENOSTR:
         strncpy(message, "[XSR] [Option Start] Not a STREAM. [Option End]", size);
         break;
#endif
      case ENOSYS:
         strncpy(message, "Function not supported.", size);
         break;
      case ENOTCONN:
         strncpy(message, "The socket is not connected.", size);
         break;
      case ENOTDIR:
         strncpy(message, "Not a directory.", size);
         break;
#if !defined(AIX) && !defined(AIX5)
      case ENOTEMPTY:
         strncpy(message, "Directory not empty.", size);
         break;
#endif
      case ENOTSOCK:
         strncpy(message, "Not a socket.", size);
         break;
      case ENOTSUP:
         strncpy(message, "Not supported.", size);
         break;
      case ENOTTY:
         strncpy(message, "Inappropriate I/O control operation.", size);
         break;
      case ENXIO:
         strncpy(message, "No such device or address.", size);
         break;
#if !defined(LINUX) && !defined(MACOSX) && !defined(FREEBSD)
      case EOPNOTSUPP:
         strncpy(message, "Operation not supported on socket.", size);
         break;
#endif
#if !defined(MG_VMS) && !defined(OSF1)
      case EOVERFLOW:
         strncpy(message, "Value too large to be stored in data type.", size);
         break;
#endif
      case EPERM:
         strncpy(message, "Operation not permitted.", size);
         break;
      case EPIPE:
         strncpy(message, "Broken pipe.", size);
         break;
#if !defined(MG_VMS) && !defined(MACOSX) && !defined(FREEBSD)
      case EPROTO:
         strncpy(message, "Protocol error.", size);
         break;
#endif
      case EPROTONOSUPPORT:
         strncpy(message, "Protocol not supported.", size);
         break;
      case EPROTOTYPE:
         strncpy(message, "Protocol wrong type for socket.", size);
         break;
      case ERANGE:
         strncpy(message, "Result too large.", size);
         break;
      case EROFS:
         strncpy(message, "Read-only file system.", size);
         break;
      case ESPIPE:
         strncpy(message, "Invalid seek.", size);
         break;
      case ESRCH:
         strncpy(message, "No such process.", size);
         break;
      case ESTALE:
         strncpy(message, "Reserved.", size);
         break;
#if !defined(MG_VMS) && !defined(MACOSX) && !defined(FREEBSD)
      case ETIME:
         strncpy(message, "[XSR] [Option Start] Stream ioctl() timeout. [Option End]", size);
         break;
#endif
      case ETIMEDOUT:
         strncpy(message, "Connection timed out.", size);
         break;
      case ETXTBSY:
         strncpy(message, "Text file busy.", size);
         break;
#if !defined(LINUX) && !defined(AIX) && !defined(AIX5) && !defined(MACOSX) && !defined(OSF1) && !defined(SOLARIS) && !defined(FREEBSD)
      case EWOULDBLOCK:
         strncpy(message, "Operation would block.", size);
         break;
#endif
      case EXDEV:
         strncpy(message, "Cross-device link.", size);
         break;
      default:
         strcpy(message, "");
      break;
   }
#endif

   return (int) strlen(message);
}

