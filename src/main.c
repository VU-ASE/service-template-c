#include "roverlib.h"
#include <sys/time.h>
#include <unistd.h>

long long current_time_millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}


// The main user space program
// this program has all you need from roverlib: service identity, reading, writing and configuration
int user_program(Service service, Service_configuration *configuration) {
    if (configuration == NULL) {
        printf("Configuration is NULL\n");
        return 1;
    }

	//
	// Access the service identity, who am I?
	//
  printf("Hello world, a new C service '%s' was born at version %s\n", service.name, service.version);

  //
	// Access the service configuration, to use runtime parameters
	//
  double *example_num = get_float_value_safe(configuration, "number-example");
  if (example_num == NULL) {
    printf("Could not get number-example from configuration\n");
    return 1;
  } 
  printf("Got number-example from configuration: %f\n", *example_num);

  char *example_str = get_string_value_safe(configuration, "string-example");
  if (example_str == NULL) {
    printf("Could not get string-example from configuration\n");
    return 1;
  }
  printf("Got string-example from configuration: %s\n", example_str);

  char *example_str_tunable = get_string_value_safe(configuration, "tunable-string-example");
  if (example_str_tunable == NULL) {
    printf("Could not get tunable-string-example from configuration\n");
    return 1;
  }
  printf("Got tunable-string-example from configuration: %s\n", example_str_tunable);

  //
	// Writing to an output that other services can read (see service.yaml to understand the output name)
	//
  write_stream *w_stream = get_write_stream(&service, "example-output");
  if (w_stream == NULL) {
    printf("Could not get write stream for output example-output\n");
  }

  // Try to write a simple rovercom message, as if we are sending RPM data
  // (see https://github.com/VU-ASE/rovercom/blob/main/definitions/outputs/wrapper.proto)
  ProtobufMsgs__SensorOutput msg = PROTOBUF_MSGS__SENSOR_OUTPUT__INIT;
  // Set the message fields
  msg.sensorid = 505;                      // let the receiver know who we are! (if you have multiple sensors in one service)
  msg.timestamp = current_time_millis();   // current time in milliseconds since epoch (useful for debugging)
  msg.status = 0;                          // we are chilling
  // Set the oneof field contents
  ProtobufMsgs__RpmSensorOutput rpm_output = PROTOBUF_MSGS__RPM_SENSOR_OUTPUT__INIT;
  rpm_output.leftrpm = 1000;
  rpm_output.rightrpm = 2400;
  // Set the oneof field (union)
  msg.rpmouput = &rpm_output;
  msg.sensor_output_case = PROTOBUF_MSGS__SENSOR_OUTPUT__SENSOR_OUTPUT_RPM_OUPUT;

  int res = write_pb(w_stream, &msg);
  if (res != 0) {
    printf("Could not write to output example-output\n");
    return 1l;
  }

	// You don't like using protobuf messages? No problem, you can write raw bytes too
  char data[] = "Hello world!\0";
  res = write_bytes(w_stream, data, sizeof(data));
  if (res != 0) {
    printf("Could not write to output example-output\n");
    return 1;
  }

	//
	// Reading from an input, to get data from other services (see service.yaml to understand the input name)
	//
  read_stream *r_stream = get_read_stream(&service, "example-input", "rpm-data");
  if (r_stream == NULL) {
    printf("Could not get read stream for input example-input\n");
  }

	// Try to read a simple rovercom message, as if we are receiving RPM data
  ProtobufMsgs__SensorOutput *rec_msg = read_pb(r_stream);
  if (rec_msg == NULL) {
    printf("Could not read from input example-input\n");
  } else {
		// Find out if we actually have rpm data
    if (rec_msg->sensor_output_case == PROTOBUF_MSGS__SENSOR_OUTPUT__SENSOR_OUTPUT_RPM_OUPUT) {
      printf("Received RPM data: left=%f, right=%f\n", rec_msg->rpmouput->leftrpm, rec_msg->rpmouput->rightrpm);
    } else {
      printf("Received data, but not RPM data\n");
    }
  }

	//
	// Now do something else fun, see if our "example-string-tunable" is updated
	//
  char *curr = strdup(example_str_tunable);
  while (true) {
    printf("Checking for tunable string update");

    // We are not using the safe version here, because using locks is boring
		// (this is perfectly fine if you are constantly polling the value)
		// nb: this is not a blocking call, it will return the last known value
    char *new_str = get_string_value(configuration, "tunable-string-example");
    if (new_str == NULL) {
      printf("Could not get tunable-string-example from configuration\n");
      continue;
    }

    if (strcmp(curr, new_str) != 0) {
      printf("Tunable string updated: %s -> %s\n", curr, new_str);
      free(curr);
      curr = strdup(new_str);
    }

    // Don't waste CPU cycles
    sleep(1);
  }

  return 0;
}

// This is just a wrapper to run the user program
// it is not recommended to put any other logic here
int main() {
  return run(user_program);
}