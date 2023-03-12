package fuzz

import (
	"fmt"
	"os/exec"
	"strings"
	"testing"
)

func doit(s string) error {
	cmd := exec.Command("../fuzz.prog", fmt.Sprintf("%d", len(s)))
	wr, err := cmd.StdinPipe()
	if err != nil {
		return err
	}
	defer wr.Close()
	if _, err := wr.Write([]byte(s)); err != nil {
		return err
	}
	data, err := cmd.CombinedOutput()
	if err != nil {
		data := strings.TrimSpace(string(data))
		if data != "" {
			panic(data)
		} else {
			panic(err.Error())
		}
	}

	return nil
}

func FuzzJSON(f *testing.F) {
	testcases := []string{
		"fa99l\xe1e",
		"Ȁ",
		"0",
		"0000<0000>",
		`[true,1,false,null,{"a":"b"},[1,2,3,4]]`,
		"true",
		"false",
		"123.01",
		"-0.1e9",
		"-0.1e+9",
		"-0.1e-9",
	}
	for _, tc := range testcases {
		f.Add(tc)
	}
	f.Fuzz(func(t *testing.T, orig string) {
		if err := doit(orig); err != nil {
			t.Fatal(err)
		}
	})
}
